from setuptools import setup
import os
import subprocess
from pathlib import Path

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension  # , build_ext
from setuptools import find_packages
from setuptools.command.build_ext import build_ext

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension(
        "xrpl_plugin.rippled_py",
        ["src/main.cpp"],
        cxx_std=17,
    ),
]

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()


class CMakeBuild(build_ext):
    def build_extension(self, ext) -> None:
        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)  # type: ignore[no-untyped-call]
        extdir = ext_fullpath.parent.resolve()

        # Using this requires trailing slash for auto-detection & inclusion of
        # auxiliary "native" libs

        # debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        debug = True
        cfg = "Debug" if debug else "Release"

        # CMake lets you override the generator - we need to check this.
        # Can be set with Conda-Build, for example.
        # cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
            "-DBUILD_SHARED_LIBS=YES",
        ]
        build_args = []
        # Adding CMake arguments set as environment variable
        # (needed e.g. to build for ARM OSx on conda-forge)
        if "CMAKE_ARGS" in os.environ:
            cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ") if item]

        # In this example, we pass in the version to C++. You might not need to.
        cmake_args += [f"-DEXAMPLE_VERSION_INFO={self.distribution.get_version()}"]  # type: ignore[attr-defined]

        # if self.compiler.compiler_type != "msvc":
        #     # Using Ninja-build since it a) is available as a wheel and b)
        #     # multithreads automatically. MSVC would require all variables be
        #     # exported for Ninja to pick it up, which is a little tricky to do.
        #     # Users can override the generator with CMAKE_GENERATOR in CMake
        #     # 3.15+.
        #     if not cmake_generator or cmake_generator == "Ninja":
        #         try:
        #             import ninja  # noqa: F401

        #             ninja_executable_path = Path(ninja.BIN_DIR) / "ninja"
        #             cmake_args += [
        #                 "-GNinja",
        #                 f"-DCMAKE_MAKE_PROGRAM:FILEPATH={ninja_executable_path}",
        #             ]
        #         except ImportError:
        #             pass

        # else:

        #     # Single config generators are handled "normally"
        #     single_config = any(x in cmake_generator for x in {"NMake", "Ninja"})

        #     # CMake allows an arch-in-generator style for backward compatibility
        #     contains_arch = any(x in cmake_generator for x in {"ARM", "Win64"})

        #     # Specify the arch if using MSVC generator, but only if it doesn't
        #     # contain a backward-compatibility arch spec already in the
        #     # generator name.
        #     if not single_config and not contains_arch:
        #         cmake_args += ["-A", PLAT_TO_CMAKE[self.plat_name]]

        #     # Multi-config generators have a different way to specify configs
        #     if not single_config:
        #         cmake_args += [
        #             f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}"
        #         ]
        #         build_args += ["--config", cfg]

        # if sys.platform.startswith("darwin"):
        #     # Cross-compile support for macOS - respect ARCHFLAGS if set
        #     archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
        #     if archs:
        #         cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                # CMake 3.12+ only.
                build_args += [f"-j{self.parallel}"]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)

        conan_cmake = "build/generators/conan_toolchain.cmake"
        if not os.path.exists(os.path.join(os.getcwd(), conan_cmake)):
            subprocess.run(
                [
                    "conan",
                    "install",
                    os.getcwd(),
                    "--build",
                    "missing",
                    "--settings",
                    f"build_type={cfg}",
                ],
                cwd=build_temp,
                check=True,
            )
        cmake_args += [f"-DCMAKE_TOOLCHAIN_FILE:FILEPATH={conan_cmake}"]
        subprocess.run(["cmake", os.getcwd()] + cmake_args, cwd=build_temp, check=True)
        subprocess.run(
            ["cmake", "--build", "."] + build_args, cwd=build_temp, check=True
        )


setup(
    name="xrpl-plugin",
    version="0.2.3",
    author="Mayukha Vadari",
    author_email="mvadari@ripple.com",
    url="https://github.com/mvadari/xrpl-plugin",
    description="XRPL Plugins",
    long_description=long_description,
    long_description_content_type="text/markdown",
    packages=find_packages(),
    include_package_data=True,
    ext_modules=ext_modules,
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    python_requires=">=3.8",
    entry_points="""
        [console_scripts]
        plugin-build=xrpl_plugin:build
    """,
)
