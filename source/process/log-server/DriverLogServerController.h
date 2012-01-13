#ifndef DRIVER_LOG_SERVER_CONTROLLER_H_
#define DRIVER_LOG_SERVER_CONTROLLER_H_

#include "LogServerStorage.h"

#include <common/Keys.h>

#include <util/driver/Controller.h>
#include <util/driver/writers/JsonWriter.h>

#include <iostream>
#include <set>

using namespace izenelib::driver;

namespace sf1r
{

/**
 * @brief Controller \b log_server
 * Log server for updating cclog
 */
class DriverLogServerHandler;
class DriverLogServerController : public izenelib::driver::Controller
{
public:
    DriverLogServerController(const boost::shared_ptr<DriverLogServerHandler>& driverLogServerHandler)
    {
        dirverLogServerHandler_ = driverLogServerHandler;
        BOOST_ASSERT(dirverLogServerHandler_);
    }

    /// overwrite
    bool preprocess();

    void update_cclog();

    void update_scd();

    void flush();

private:
    boost::shared_ptr<DriverLogServerHandler> dirverLogServerHandler_;
};


class DriverLogServerHandler
{
public:
    void setRequestContext(
            Request& request,
            Response& response)
    {
        request_ = &request;
        response_ = &response;
    }

    Request& request()
    {
        return *request_;
    }

    const Request& request() const
    {
        return *request_;
    }

    Response& response()
    {
        return *response_;
    }

    const Response& response() const
    {
        return *response_;
    }

public:
    void init();

    void processCclog();

    void processScd();

    void flush();

private:
    bool skipProcess(const std::string& collection);

    void processDocVisit(izenelib::driver::Value& request, const std::string& raw);

    void processRecVisitItem(izenelib::driver::Value& request, const std::string& raw);

    void processRecPurchaseItem(izenelib::driver::Value& request, const std::string& raw);

    void onUniqueKeyCheck(
            const LogServerStorage::drum_key_t& uuid,
            const LogServerStorage::drum_value_t& docidList,
            const LogServerStorage::drum_aux_t& aux);

    void onDuplicateKeyCheck(
            const LogServerStorage::drum_key_t& uuid,
            const LogServerStorage::drum_value_t& docidList,
            const LogServerStorage::drum_aux_t& aux);

    void ouputCclog(const std::string& log);

    struct CCLogMerge
    {
        std::size_t uuidCnt_;
        std::string request_;
        std::vector<std::pair<uint128_t, std::set<uint128_t> > > uuidUpdateVec_;

        CCLogMerge(std::size_t uuidCnt = 0, const std::string& request = "")
            : uuidCnt_(uuidCnt)
            , request_(request)
        {}
    };

    void mergeCClog(boost::shared_ptr<CCLogMerge>& cclogMergeUnit);

private:
    Request* request_;
    Response* response_;
    izenelib::driver::JsonWriter jsonWriter_;

    std::set<std::string> driverCollections_;

    std::string cclogFileName_;
    boost::shared_ptr<std::ofstream> cclogFile_;
    boost::mutex cclog_file_mutex_;

    std::map<uint128_t, boost::shared_ptr<CCLogMerge> > cclogMergeQueue_;
    boost::mutex cclog_merge_mutex_;
};

}

#endif /* DRIVER_LOG_SERVER_CONTROLLER_H_ */
