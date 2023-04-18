use cxx::let_cxx_string;

#[cxx::bridge]
mod rippled {
    // These are Rust functions that can be called by C++
    extern "Rust" {
        /*type MultiBuf;

        fn next_chunk(buf: &mut MultiBuf) -> &[u8];*/
    }

    // These are C++ functions that can be called by Rust
    #[namespace = "ripple"]
    unsafe extern "C++" {
        // include!("rust/include/blobstore.h");
        include!("rust/include/blobstore.h");

        // pub(crate) fn encoded_size(n: usize) -> usize;

        /*include!("rust/include/blobstore.h");

        type BlobstoreClient;


        pub(crate) fn new_blobstore_client() -> UniquePtr<BlobstoreClient>;
        pub(crate) fn put(&self, parts: &mut MultiBuf) -> u64;*/
    }

    unsafe extern "C++" {
        pub(crate) fn base64_decode_ptr(s: &CxxString) -> UniquePtr<CxxString>;
    }
}

/*pub struct MultiBuf {
    chunks: Vec<Vec<u8>>,
    pos: usize,
}

pub fn next_chunk(buf: &mut MultiBuf) -> &[u8] {
    let next = buf.chunks.get(buf.pos);
    buf.pos += 1;
    next.map_or(&[], Vec::as_slice)
}*/

/*#[link(name = "xrpl_core", kind="static")]
extern "C" {
    fn encoded_size(n: usize) -> usize;
}*/

fn main() {
    /*let client = ffi::new_blobstore_client();

    let chunks = vec![b"fearless".to_vec(), b"concurrency".to_vec()];
    let mut buf = MultiBuf { chunks, pos: 0 };
    let blobid = client.put(&mut buf);
    println!("blobid = {}", blobid);*/
    // let size = unsafe {
    //     rippled::encoded_size(4)
    // };
    // println!("Size: {}", size);

    let_cxx_string!(b64 = "dGhlIGNha2UgaXMgYSBsaWU");
    let decoded = rippled::base64_decode_ptr(&b64);
    println!("decoded: {}", decoded)
}
