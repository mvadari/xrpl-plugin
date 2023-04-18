//
// Created by Noah Kramer on 4/17/23.
//

#include "rust/include/blobstore.h"
#include "ripple/basics/base64.h"
#include "rust/src/main.rs.h"
#include <functional>
#include <string>

std::unique_ptr<std::string>
base64_decode_ptr(std::string const& data)
{
    return std::make_unique<std::string>(ripple::base64_decode(data));
}

/*std::size_t constexpr encoded_size(std::size_t n) {
return 4 * (( n + 2 ) / 3 ) ;
}*/

/*BlobstoreClient::BlobstoreClient() {}

std::unique_ptr<BlobstoreClient> new_blobstore_client() {
    return std::unique_ptr<BlobstoreClient>(new BlobstoreClient());
}

// Upload a new blob and return a blobid that serves as a handle to the blob.
uint64_t BlobstoreClient::put(MultiBuf &buf) const {
    // Traverse the caller's chunk iterator.
    std::string contents;
    while (true) {
        auto chunk = next_chunk(buf);
        if (chunk.size() == 0) {
            break;
        }
        contents.append(reinterpret_cast<const char *>(chunk.data()), chunk.size());
    }

    // Pretend we did something useful to persist the data.
    auto blobid = std::hash<std::string>{}(contents);
    return blobid;
}*/
