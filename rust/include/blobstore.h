//
// Created by Noah Kramer on 4/17/23.
//

#ifndef PLUGIN_TRANSACTOR_BLOBSTORE_H
#define PLUGIN_TRANSACTOR_BLOBSTORE_H

#pragma once

#include <memory>
#include "ripple/basics/base64.h"

std::unique_ptr<std::string>
base64_decode_ptr(std::string const& data);

/*struct MultiBuf;

class BlobstoreClient {
public:
    BlobstoreClient();

    uint64_t put(MultiBuf &buf) const;
};

std::unique_ptr <BlobstoreClient> new_blobstore_client();*/

#endif //PLUGIN_TRANSACTOR_BLOBSTORE_H
