/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

///@internal
#include "proton/types_fwd.hpp"

#ifndef LINK_NAMER_HPP
#define LINK_NAMER_HPP

namespace proton {

/// @cond INTERNAL
class link_namer {
  public:
    link_namer(const std::string &prefix="");
    std::string next();
    void prefix(const std::string &p) { prefix_ = p; }
    const std::string& prefix() const { return prefix_; }

  private:
    std::string prefix_;
    uint64_t count_;
};

}

/// @endcond

#endif