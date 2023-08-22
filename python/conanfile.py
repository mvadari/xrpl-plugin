from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
import re


class TryXrpl(ConanFile):
    name = "try-xrpl"

    license = "ISC"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "fPIC": [True, False],
        "shared": [True, False],
    }

    requires = [
        "boost/1.77.0",
        "date/3.0.1",
        "libarchive/3.6.0",
        "lz4/1.9.3",
        "grpc/1.50.1",
        "nudb/2.0.8",
        "openssl/1.1.1m",
        "protobuf/3.21.4",
        "pybind11/2.10.4",
        "rocksdb/6.27.3",
        "snappy/1.1.9",
        "soci/4.0.3",
        "sqlite3/3.38.0",
        "zlib/1.2.12",
    ]

    default_options = {
        "fPIC": True,
        "shared": False,
        "cassandra-cpp-driver:shared": False,
        "date:header_only": True,
        "grpc:shared": False,
        "grpc:secure": True,
        "libarchive:shared": False,
        "libarchive:with_acl": False,
        "libarchive:with_bzip2": False,
        "libarchive:with_cng": False,
        "libarchive:with_expat": False,
        "libarchive:with_iconv": False,
        "libarchive:with_libxml2": False,
        "libarchive:with_lz4": True,
        "libarchive:with_lzma": False,
        "libarchive:with_lzo": False,
        "libarchive:with_nettle": False,
        "libarchive:with_openssl": False,
        "libarchive:with_pcreposix": False,
        "libarchive:with_xattr": False,
        "libarchive:with_zlib": False,
        "libpq:shared": False,
        "lz4:shared": False,
        "openssl:shared": False,
        "protobuf:shared": False,
        "protobuf:with_zlib": True,
        "rocksdb:enable_sse": False,
        "rocksdb:lite": False,
        "rocksdb:shared": False,
        "rocksdb:use_rtti": True,
        "rocksdb:with_jemalloc": False,
        "rocksdb:with_lz4": True,
        "rocksdb:with_snappy": True,
        "snappy:shared": False,
        "soci:shared": False,
        "soci:with_sqlite3": True,
        "soci:with_boost": True,
    }

    def configure(self):
        if self.settings.compiler == "apple-clang":
            self.options["boost"].visibility = "global"

    exports_sources = "CMakeLists.txt", "src/*"

    def layout(self):
        cmake_layout(self)
        # Fix this setting to follow the default introduced in Conan 1.48
        # to align with our build instructions.
        self.folders.generators = "build/generators"

    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.install()
