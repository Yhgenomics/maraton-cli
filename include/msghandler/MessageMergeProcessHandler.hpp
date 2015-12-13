#ifndef MESSAGE_MERGE_PROCESS_HANDLER_HPP_
#define MESSAGE_MERGE_PROCESS_HANDLER_HPP_

#include "MessageMergeProcess.hpp"
#include "AsyncWorker.h"
#include "ExecutorConfig.hpp"
#include "FileUploader.h"
#include "FileDownloader.h"
#include "AnalysisHelper.h"
#include <vector>
#include "MessageTaskResult.hpp"

namespace Protocol
{
    static  void    PullBamBegin ( AsyncWorker* asyncWorker ); // download files
    static  void    PullBamEnd   ( AsyncWorker* asyncWorker ); // invoke mergebegin
    static  void    MergeBegin   ( AsyncWorker* asyncWorker ); // invoke docker
    static  void    MergeEnd     ( AsyncWorker* asyncWorker ); // upload

    static int MessageMergeProcessHandler( MessageMergeProcess msg )
    {
        // UserDefineHandler Begin
        // Your Codes here!
        cout<< "############ merge in" << endl;
        if ( PostOffice::instance()->self_status == 3 )
        {
            PostOffice::instance()->self_status = PostOffice::ExcutorSates::kComputing;
            PostOffice::instance()->SendSelfStatus();

           // PostOffice::instance()->current_task                                = msg.task_id();
           // PostOffice::instance()->task_board[ msg.task_id() ].task_id         = msg.task_id();
           // PostOffice::instance()->task_board[ msg.task_id() ].ref_gen_name    = msg.reference();

            if( 1 == msg.uri_list().size() )
            {
                cout << "######### only one file !"<< endl;
            }
            else
            {
                auto indata = new MessageMergeProcess( msg );
               // for ( auto uri : msg.uri_list() )
               // {
                // PostOffice::instance()->task_board[ msg.task_id() ].block_map[ uri ] = TaskStatus::BlockStatus::kWait;
               //     *bams += ( uri + " " );
               // }
                AsyncWorker::create( PullBamBegin    , PullBamEnd    , indata );
            }

           // AsyncWorker::create( PullSamBegin    , PullSamEnd    , bams );
           // AsyncWorker::create( ProcessBegin , ProcessEnd , nullptr );
           // AsyncWorker::create( InspectOn    , InspectOff , nullptr );
        }

        return 0;
        // UserDefineHandler End 
    }

    static  void    PullBamBegin    ( AsyncWorker* asyncWorker )
    {
        cout << endl << "############ Pull Bam Begin" << endl;
        auto msg = static_cast< MessageMergeProcess* >( asyncWorker->data() );
        for ( auto item : msg->uri_list() )
        {
            bool cancel = false;
            string fileName =item;
            ExecutorConfig::instance()->GetFileName( fileName, ExecutorConfig::SuffixType::kUnMergeFile );
            string uri = "";
            ExecutorConfig::instance()->GetUri( uri, ExecutorConfig::instance()->pullBam, item, "" );
            FileDownloader fileDownloader( &cancel );

            fileDownloader.DownloadViaHTTP( ExecutorConfig::instance()->inputDir + fileName  , uri );
        }
        auto indata = new MessageMergeProcess( *msg );
        AsyncWorker::create( MergeBegin    , MergeEnd    , indata  );
        delete msg;
        msg = nullptr;
    }

    static  void    PullBamEnd   ( AsyncWorker* asyncWorker )
    {
        cout << endl << "############ Pull Bam all done!" << endl;
    }

    static  void    MergeBegin   ( AsyncWorker* asyncWorker )
    {
        auto msg = static_cast< MessageMergeProcess* > ( asyncWorker->data() );

        MaratonCommon::AnalysisHelper analysisHelper;
        string bams = "";
        for( auto bam : msg->uri_list() )
        {
            bams += ( "/input/" + bam + ExecutorConfig::instance()->unmergeTail + " " );
        }
        cout <<"bams : "<< bams <<endl;
        auto exitStatus=  analysisHelper.MergeProcess( "hg19.fa"
               , bams
               , msg->task_id()
                );

        cout << exitStatus <<" #########vcf finished " << msg->task_id()+".vcf" << endl;

        FileUploader uploader;
        uploader.UploadFileViaHttp( msg->task_id()
                , ExecutorConfig::instance()->outputDir + msg->task_id()+".vcf"
                , ExecutorConfig::instance()->postVcf );

        delete msg;
        msg = nullptr;

    }

    static  void    MergeEnd     ( AsyncWorker* asyncWorker )
    {
        MessageTaskResult msgout;

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kTaskFinished;
        PostOffice::instance()->SendSelfStatus();

        cout << endl << "############ merged" << endl;

        msgout.task_id( PostOffice::instance()->current_task );
        if ( PostOffice::instance()->GetCurrentTaskStatus().IsAnyException() )
        {
            msgout.error( 1 );
            msgout.stage( ExecutorConfig::instance()->TaskStage::kUnknown );
            msgout.result( "[ EXCEPTION ]: At least one block in the list cannot be processed" );
            cout <<endl << "######### but At least one block in the list cannot be processed " << endl;
        }
        else
        {
            msgout.error( 0 );
            msgout.stage ( ExecutorConfig::instance()->TaskStage::kMerged );
            msgout.result( "OK! Merge finished!" );
        }

        PostOffice::instance()->SendMail( &msgout );

        PostOffice::instance()->self_status = PostOffice::ExcutorSates::kStandby;
        PostOffice::instance()->SendSelfStatus();
        cout << endl << "############ Standby" << endl;
    }

} // End of namespace Protocol
#endif // !Message_Merge_Process_HANDLER_HPP_
