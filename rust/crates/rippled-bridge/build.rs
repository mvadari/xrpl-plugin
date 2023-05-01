use std::path::{Path, PathBuf};

fn main() {
    let manifest_dir = env!("CARGO_MANIFEST_DIR");
    let target_dir = Path::new(&manifest_dir)
        .join("target")
        .join("rippled");
    std::fs::create_dir_all(&target_dir).unwrap();

    remove_and_copy_file(None, "libxrpl_core.a", &target_dir);
    remove_and_copy_file(Some("src/secp256k1"), "libsecp256k1.a", &target_dir);
    remove_and_copy_file(Some("src/ed25519-donna"), "libed25519.a", &target_dir);

    println!("cargo:rustc-link-search=native={}/target/rippled/", manifest_dir);
    println!("cargo:rustc-link-lib=xrpl_core");
    println!("cargo:rustc-link-lib=secp256k1");
    println!("cargo:rustc-link-lib=ed25519");
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/rippled_api.cpp");
    println!("cargo:rerun-if-changed=include/rippled_api.h");

    println!("cargo:rustc-link-search=native={}", "/Users/nkramer/.conan/data/boost/1.77.0/_/_/package/1080f5eeec2c2f1a57638424f85f00e32203faa4/lib/");
    println!("cargo:rustc-link-lib=boost_thread");

    println!("cargo:rustc-link-search=native={}", "/Users/nkramer/.conan/data/openssl/1.1.1m/_/_/package/240c2182163325b213ca6886a7614c8ed2bf1738/lib/");
    println!("cargo:rustc-link-lib=crypto");

    cxx_build::bridge("src/lib.rs")
        .file("src/rippled_api.cpp")
        .flag_if_supported("-std=c++20")
        .flag("-DBOOST_ASIO_HAS_STD_INVOKE_RESULT")
        // TODO: The boost and date include paths are hardcoded for my machine. Is there
        //  a better way to include dependency header files? Maybe we try to get cxx to build
        //  with cmake and include a CMakeLists.txt that depends on boost and date? Maybe
        //  we add a conan step in this build.rs that installs boost and date somewhere
        //  and then we can include those output paths here?
        .includes([
            Path::new(manifest_dir).join("../../../external/rippled/src/"),
            Path::new("/Users/nkramer/.conan/data/boost/1.77.0/_/_/source/src/").to_path_buf(),
            Path::new("/Users/nkramer/.conan/data/date/3.0.1/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include/").to_path_buf()
        ])
        .compile("rippled_bridge");
}

fn remove_and_copy_file(extra_build_path: Option<&str>, lib_name: &str, target_dir: &PathBuf) {
    let mut lib_file = Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../external/rippled/.build/");
    if let Some(extra_path) = extra_build_path {
        lib_file = lib_file.join(extra_path);
    }

    lib_file = lib_file.join(lib_name);

    if target_dir.join(lib_name).exists() {
        std::fs::remove_file(target_dir.join(lib_name)).unwrap();
    }

    let res = std::fs::copy(lib_file.clone(), target_dir.join(lib_name).clone());
    if res.is_err() {
        println!(
            "cargo:warning=Error copying file: {} {} {:?}",
            lib_file.display(),
            target_dir.display(),
            res
        );
    }

    res.unwrap();
}