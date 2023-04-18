use std::path::Path;

fn main() {
    let manifest_dir = env!("CARGO_MANIFEST_DIR");
    let target_dir = Path::new(&manifest_dir)
        .join("target")
        .join("rippled");
    std::fs::create_dir_all(&target_dir).unwrap();

    let xrpl_core_file = Path::new(&manifest_dir)
        .join("../external/rippled/.build/libxrpl_core.a");

    std::fs::remove_file(target_dir.join("libxrpl_core.a"));
    let res = std::fs::copy(xrpl_core_file.clone(), target_dir.join("libxrpl_core.a").clone());
    if res.is_err() {
        println!(
            "cargo:warning=Error copying file: {} {} {:?}",
            xrpl_core_file.display(),
            target_dir.display(),
            res
        );
    }

    res.unwrap();

    println!("cargo:rustc-link-search=native={}/target/rippled/", manifest_dir);
    println!("cargo:rustc-link-lib=xrpl_core");
    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/blobstore.cpp");
    println!("cargo:rerun-if-changed=include/blobstore.h");

    cxx_build::bridge("src/main.rs")
        .file("src/blobstore.cpp")
        .flag_if_supported("-std=c++14")
        .include(Path::new(manifest_dir).join("../external/rippled/src/"))
        .compile("rust");
}