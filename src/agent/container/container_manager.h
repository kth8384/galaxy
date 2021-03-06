// Copyright (c) 2016, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once
#include "container.h"
#include "resource/resource_manager.h"
#include "container_stage.h"
#include "serializer.h"

#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp" 
#include "thread.h"
#include "thread_pool.h"
#include "container_gc.h"

#include <map>
#include <string>

namespace baidu {
namespace galaxy {
namespace container {

enum EvictType {
    kEvictTypeMemory = 1,
    kEvictTypeCpu = 2
};

class ContainerManager {
public:
    explicit ContainerManager(boost::shared_ptr<baidu::galaxy::resource::ResourceManager> resman);
    ~ContainerManager();

    void Setup();
    baidu::galaxy::util::ErrorCode CreateContainer(const ContainerId& id,
            const baidu::galaxy::proto::ContainerDescription& desc);

    baidu::galaxy::util::ErrorCode ReleaseContainer(const ContainerId& id);
    void ListContainers(std::vector<boost::shared_ptr<baidu::galaxy::proto::ContainerInfo> >& cis, bool fullinfo);

private:
    baidu::galaxy::util::ErrorCode DependentVolums(const baidu::galaxy::proto::ContainerDescription& desc,
            std::map<std::string, std::string>& dv);
    // key: source path, value: target path
    baidu::galaxy::util::ErrorCode DependentVolums(const ContainerId& id,
            std::map<std::string, std::string>& volums,         // key: source; value: target
            std::map<std::string, std::string>& check_volums);  // key: targe;  value: source, make sure uniq targe

    baidu::galaxy::util::ErrorCode CreateContainer_(const ContainerId& id,
            const baidu::galaxy::proto::ContainerDescription& desc);

    void KeepAliveRoutine();
    void CheckAssignRoutine();
    void EvictAssignedContainer(
        std::vector<boost::shared_ptr<baidu::galaxy::proto::ContainerInfo> >& cis,
        EvictType evict_type);
    int Reload();
    void DumpProperty(boost::shared_ptr<IContainer> container);

    std::map<ContainerId, boost::shared_ptr<baidu::galaxy::container::IContainer> > work_containers_;
    //boost::scoped_ptr<baidu::common::ThreadPool> check_read_threadpool_;
    boost::shared_ptr<baidu::galaxy::resource::ResourceManager> res_man_;
    boost::mutex mutex_;

    ContainerStage stage_;

    std::vector<boost::shared_ptr<baidu::galaxy::proto::ContainerInfo> > container_infos_;
    baidu::common::Thread keep_alive_thread_;
    baidu::common::ThreadPool check_assign_pool_;
    bool running_;

    boost::shared_ptr<Serializer> serializer_;
    boost::shared_ptr<ContainerGc> container_gc_;
};

} //namespace agent
} //namespace galaxy
} //namespace baidu
