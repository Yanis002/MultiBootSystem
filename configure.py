#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "mq-j",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.lower,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-1"
config.compilers_tag = "20240706"
config.dtk_tag = "v1.4.1"
config.objdiff_tag = "v2.7.1"
config.sjiswrap_tag = "v1.2.0"
config.wibo_tag = "0.6.11"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    # "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
    f"--defsym VERSION={config.version.upper().replace("-", "_")}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
    "-warn off",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    # "-W all",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI on",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION={config.version.upper().replace("-", "_")}",
    "-msgstyle gcc",
    "-sym on",
    "-i libc",
    "-i libcpp",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/1.3.2"

### Helper functions

def MenuLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": [*cflags_base, "-inline auto"],
        "progress_category": "menu",
        "objects": objects,
    }


def DolphinLib(lib_name: str, objects: List[Object], mw_version: str = "GC/1.2.5n") -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": mw_version,
        "cflags": cflags_base,
        "progress_category": "dolphin",
        "objects": objects,
    }


def JSystemLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": [
            *cflags_base,
            "-use_lmw_stmw off",
            "-str reuse,pool,readonly",
            "-inline noauto",
            "-O3,s",
            "-schedule off",
            "-sym on",
            "-fp_contract off",
        ],
        "progress_category": "jsystem",
        "objects": objects,
    }


def GenericLib(lib_name: str, cflags: List[str], objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags,
        "progress_category": lib_name,
        "objects": objects,
    }


Linked = True                     # Object matches and should be linked
NotLinked = False                 # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def LinkedFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    MenuLib(
        "menu",
        [
            Object(NotLinked, "menu/main.cpp"),
            Object(LinkedFor("mq-j"), "menu/dtk_stuff.cpp"),
            Object(LinkedFor("mq-j"), "menu/graphics.cpp"),
            Object(LinkedFor("mq-j"), "menu/mixedcontroller.cpp"),
            Object(LinkedFor("mq-j"), "menu/mtrand.cpp"),
            Object(LinkedFor("mq-j"), "menu/discerror.c"),
            Object(NotLinked, "menu/soundeffect.cpp"),
        ]
    ),
    DolphinLib(
        "exec",
        [
            Object(NotLinked, "dolphin/exec/exec.c"),
        ],
        "GC/1.2.5" # TODO: figure out if this is real
    ),
    DolphinLib(
        "base",
        [
            Object(LinkedFor("mq-j"), "dolphin/base/PPCArch.c"),
        ]
    ),
    DolphinLib(
        "os",
        [
            Object(LinkedFor("mq-j"), "dolphin/os/OS.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSAlarm.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSAlloc.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSArena.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSAudioSystem.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSCache.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSContext.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSError.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSFont.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSInterrupt.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSLink.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSMessage.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSMemory.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSMutex.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSReboot.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSReset.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSResetSW.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSRtc.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSSync.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSThread.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/OSTime.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/__start.c"),
            Object(LinkedFor("mq-j"), "dolphin/os/__ppc_eabi_init.c"),
        ]
    ),
    DolphinLib(
        "exi",
        [
            Object(LinkedFor("mq-j"), "dolphin/exi/EXIBios.c"),
            Object(LinkedFor("mq-j"), "dolphin/exi/EXIUart.c"),
        ]
    ),
    DolphinLib(
        "si",
        [
            Object(LinkedFor("mq-j"), "dolphin/si/SIBios.c"),
            Object(LinkedFor("mq-j"), "dolphin/si/SISamplingRate.c"),
        ]
    ),
    DolphinLib(
        "db",
        [
            Object(LinkedFor("mq-j"), "dolphin/db/db.c"),
        ]
    ),
    DolphinLib(
        "mtx",
        [
            Object(LinkedFor("mq-j"), "dolphin/mtx/mtx.c"),
            Object(LinkedFor("mq-j"), "dolphin/mtx/mtxvec.c"),
            Object(LinkedFor("mq-j"), "dolphin/mtx/mtx44.c"),
            Object(LinkedFor("mq-j"), "dolphin/mtx/vec.c"),
        ]
    ),
    DolphinLib(
        "dvd",
        [
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvdlow.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvdfs.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvd.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvdqueue.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvderror.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvdidutils.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/dvdFatal.c"),
            Object(LinkedFor("mq-j"), "dolphin/dvd/fstload.c"),
        ]
    ),
    DolphinLib(
        "vi",
        [
            Object(LinkedFor("mq-j"), "dolphin/vi/vi.c"),
        ]
    ),
    DolphinLib(
        "pad",
        [
            Object(LinkedFor("mq-j"), "dolphin/pad/Padclamp.c"),
            Object(LinkedFor("mq-j"), "dolphin/pad/Pad.c"),
        ]
    ),
    DolphinLib(
        "ai",
        [
            Object(LinkedFor("mq-j"), "dolphin/ai/ai.c"),
        ]
    ),
    DolphinLib(
        "ar",
        [
            Object(LinkedFor("mq-j"), "dolphin/ar/ar.c"),
            Object(LinkedFor("mq-j"), "dolphin/ar/arq.c"),
        ]
    ),
    DolphinLib(
        "ax",
        [
            Object(NotLinked, "dolphin/ax/AX.c"),
            Object(NotLinked, "dolphin/ax/AXAlloc.c"),
            Object(NotLinked, "dolphin/ax/AXAux.c"),
            Object(NotLinked, "dolphin/ax/AXCL.c"),
            Object(NotLinked, "dolphin/ax/AXOut.c"),
            Object(NotLinked, "dolphin/ax/AXSPB.c"),
            Object(NotLinked, "dolphin/ax/AXVPB.c"),
            Object(NotLinked, "dolphin/ax/AXComp.c"),
            Object(NotLinked, "dolphin/ax/DSPCode.c"),
            Object(NotLinked, "dolphin/ax/AXProf.c"),
        ]
    ),
    DolphinLib(
        "mix",
        [
            Object(NotLinked, "dolphin/mix/mix.c"),
        ]
    ),
    DolphinLib(
        "sp",
        [
            Object(NotLinked, "dolphin/sp/sp.c"),
        ]
    ),
    DolphinLib(
        "am",
        [
            Object(NotLinked, "dolphin/am/am.c"),
        ]
    ),
    DolphinLib(
        "dsp",
        [
            Object(LinkedFor("mq-j"), "dolphin/dsp/dsp.c"),
            Object(LinkedFor("mq-j"), "dolphin/dsp/dsp_debug.c"),
            Object(LinkedFor("mq-j"), "dolphin/dsp/dsp_task.c"),
        ]
    ),
    DolphinLib(
        "dtk",
        [
            Object(NotLinked, "dolphin/dtk/dtk.c"),
        ]
    ),
    DolphinLib(
        "card",
        [
            Object(LinkedFor("mq-j"), "dolphin/card/CARDBios.c"),
            Object(LinkedFor("mq-j"), "dolphin/card/CARDMount.c"),
        ]
    ),
    DolphinLib(
        "gx",
        [
            Object(NotLinked, "dolphin/gx/GXInit.c"),
            Object(NotLinked, "dolphin/gx/GXFifo.c"),
            Object(NotLinked, "dolphin/gx/GXAttr.c"),
            Object(NotLinked, "dolphin/gx/GXMisc.c"),
            Object(NotLinked, "dolphin/gx/GXGeometry.c"),
            Object(NotLinked, "dolphin/gx/GXFrameBuf.c"),
            Object(NotLinked, "dolphin/gx/GXLight.c"),
            Object(NotLinked, "dolphin/gx/GXTexture.c"),
            Object(NotLinked, "dolphin/gx/GXBump.c"),
            Object(NotLinked, "dolphin/gx/GXTev.c"),
            Object(NotLinked, "dolphin/gx/GXPixel.c"),
            Object(NotLinked, "dolphin/gx/GXStubs.c"),
            Object(NotLinked, "dolphin/gx/GXDisplayList.c"),
            Object(NotLinked, "dolphin/gx/GXTransform.c"),
            Object(NotLinked, "dolphin/gx/GXPerf.c"),
        ]
    ),
    DolphinLib(
        "gd",
        [
            Object(NotLinked, "dolphin/gd/GDBase.c"),
            Object(NotLinked, "dolphin/gd/GDGeometry.c"),
        ]
    ),
    JSystemLib(
        "JKernel",
        [
            Object(NotLinked, "JSystem/JKernel/JKRHeap.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRStdHeap.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRExpHeap.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRSolidHeap.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRDisposer.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRThread.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRAram.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRAramHeap.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRAramBlock.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRAramPiece.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRAramStream.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRFileLoader.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRFileFinder.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRArchivePub.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRArchivePri.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRMemArchive.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRAramArchive.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRDvdArchive.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRCompArchive.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRDvdFile.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRDvdRipper.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRDvdAramRipper.cpp"),
            Object(NotLinked, "JSystem/JKernel/JKRDecomp.cpp"),
        ]
    ),
    JSystemLib(
        "JSupport",
        [
            Object(NotLinked, "JSystem/JSupport/JSUList.cpp"),
            Object(NotLinked, "JSystem/JSupport/JSUInputStream.cpp"),
            Object(NotLinked, "JSystem/JSupport/JSUMemoryStream.cpp"),
            Object(NotLinked, "JSystem/JSupport/JSUFileStream.cpp"),
        ]
    ),
    JSystemLib(
        "JGadget",
        [
            Object(NotLinked, "JSystem/JGadget/linklist.cpp"),
        ]
    ),
    JSystemLib(
        "JUtility",
        [
            Object(NotLinked, "JSystem/JUtility/JUTCacheFont.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTResource.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTTexture.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTPalette.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTNameTab.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTRect.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTGraphFifo.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTFont.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTResFont.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTDbPrint.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTGamePad.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTFontData_Ascfont_fix12.s"),
            Object(NotLinked, "JSystem/JUtility/JUTException.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTDirectPrint.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTAssert.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTVideo.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTXfb.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTFader.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTProcBar.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTConsole.cpp"),
            Object(NotLinked, "JSystem/JUtility/JUTDirectFile.cpp"),
        ]
    ),
    JSystemLib(
        "J2DGraph",
        [
            Object(NotLinked, "JSystem/J2DGraph/J2DGrafContext.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DOrthoGraph.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DPrint.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DPane.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DScreen.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DWindow.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DPicture.cpp"),
            Object(NotLinked, "JSystem/J2DGraph/J2DTextBox.cpp"),
        ]
    ),
    JSystemLib(
        "JRenderer",
        [
            Object(NotLinked, "JSystem/JRenderer/JRenderer.cpp"),
        ]
    ),
    JSystemLib(
        "J3DGraphBase",
        [
            Object(NotLinked, "JSystem/J3DGraphBase/J3DGD.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DSys.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DVertex.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DTransform.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DPacket.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DShapeMtx.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DShape.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DMaterial.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DMatBlock.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DTevs.cpp"),
            Object(NotLinked, "JSystem/J3DGraphBase/J3DDrawBuffer.cpp"),
        ]
    ),
    JSystemLib(
        "J3DGraphAnimator",
        [
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DModelData.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DModel.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DAnimation.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DMaterialAnm.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DCluster.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DJoint.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DNode.cpp"),
            Object(NotLinked, "JSystem/J3DGraphAnimator/J3DMaterialAttach.cpp"),
        ]
    ),
    JSystemLib(
        "J3DGraphLoader",
        [
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DMaterialFactory.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DMaterialFactory_v21.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DClusterLoader.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DModelLoader.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DModelLoaderCalcSize.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DJointFactory.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DShapeFactory.cpp"),
            Object(NotLinked, "JSystem/J3DGraphLoader/J3DAnmLoader.cpp"),
        ]
    ),
    JSystemLib(
        "JMath",
        [
            Object(NotLinked, "JSystem/JMath/JMath.cpp"),
            Object(NotLinked, "JSystem/JMath/random.cpp"),
        ]
    ),
    JSystemLib(
        "JFramework",
        [
            Object(NotLinked, "JSystem/JFramework/JFWSystem.cpp"),
            Object(NotLinked, "JSystem/JFramework/JFWDisplay.cpp"),
        ]
    ),
    JSystemLib(
        "JPALoader",
        [
            Object(NotLinked, "JSystem/JPALoader/JPABaseShape.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPAExtraShape.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPASweepShape.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPAExTexShape.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPADynamicsBlock.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPAFieldBlock.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPAKeyBlock.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPATexture.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPAResourceManager.cpp"),
            Object(NotLinked, "JSystem/JPALoader/JPAEmitterLoader.cpp"),
        ]
    ),
    JSystemLib(
        "JPABase",
        [
            Object(NotLinked, "JSystem/JPABase/JPAMath.cpp"),
            Object(NotLinked, "JSystem/JPABase/JPAField.cpp"),
            Object(NotLinked, "JSystem/JPABase/JPAEmitter.cpp"),
            Object(NotLinked, "JSystem/JPABase/JPAParticle.cpp"),
            Object(NotLinked, "JSystem/JPABase/JPAEmitterManager.cpp"),
        ]
    ),
    JSystemLib(
        "JPADraw",
        [
            Object(NotLinked, "JSystem/JPADraw/JPADrawVisitor.cpp"),
            Object(NotLinked, "JSystem/JPADraw/JPADraw.cpp"),
            Object(NotLinked, "JSystem/JPADraw/JPADrawSetupTev.cpp"),
        ]
    ),
    GenericLib(
        "runtime",
        [*cflags_base, "-use_lmw_stmw on", "-str reuse,readonly", "-inline auto,deferred"],
        [
            Object(LinkedFor("mq-j"), "runtime/__va_arg.c"),
            Object(LinkedFor("mq-j"), "runtime/global_destructor_chain.c"),
            Object(NotLinked, "runtime/NMWException.cp"),
            Object(LinkedFor("mq-j"), "runtime/ptmf.c"),
            Object(LinkedFor("mq-j"), "runtime/runtime.c"),
            Object(LinkedFor("mq-j"), "runtime/__init_cpp_exceptions.cpp"),
            Object(LinkedFor("mq-j"), "runtime/Gecko_ExceptionPPC.cp"),
            Object(LinkedFor("mq-j"), "runtime/GCN_mem_alloc.c"),
        ]
    ),
    GenericLib(
        "libc",
        [*cflags_base, "-use_lmw_stmw on", "-str reuse,pool,readonly", "-inline deferred"],
        [
            Object(LinkedFor("mq-j"), "libc/abort_exit.c"),
            Object(NotLinked, "libc/alloc.c"),
            Object(LinkedFor("mq-j"), "libc/errno.c"),
            Object(LinkedFor("mq-j"), "libc/ansi_files.c"),
            Object(NotLinked, "libc/ansi_fp.c"),
            Object(LinkedFor("mq-j"), "libc/arith.c"),
            Object(LinkedFor("mq-j"), "libc/buffer_io.c"),
            Object(LinkedFor("mq-j"), "libc/ctype.c"),
            Object(LinkedFor("mq-j"), "libc/direct_io.c"),
            Object(LinkedFor("mq-j"), "libc/file_io.c"),
            Object(LinkedFor("mq-j"), "libc/file_pos.c"),
            Object(LinkedFor("mq-j"), "libc/mbstring.c"),
            Object(LinkedFor("mq-j"), "libc/mem.c"),
            Object(LinkedFor("mq-j"), "libc/mem_funcs.c"),
            Object(LinkedFor("mq-j"), "libc/misc_io.c"),
            Object(LinkedFor("mq-j"), "libc/printf.c"),
            Object(LinkedFor("mq-j"), "libc/float.c"),
            Object(LinkedFor("mq-j"), "libc/scanf.c"),
            Object(LinkedFor("mq-j"), "libc/string.c"),
            Object(LinkedFor("mq-j"), "libc/strtoul.c"),
            Object(LinkedFor("mq-j"), "libc/uart_console_io.c"),
            Object(LinkedFor("mq-j"), "libc/wchar_io.c"),
            Object(LinkedFor("mq-j"), "libc/e_acos.c"),
            Object(LinkedFor("mq-j"), "libc/e_atan2.c"),
            Object(LinkedFor("mq-j"), "libc/e_rem_pio2.c"),
            Object(LinkedFor("mq-j"), "libc/k_cos.c"),
            Object(LinkedFor("mq-j"), "libc/k_rem_pio2.c"),
            Object(LinkedFor("mq-j"), "libc/k_sin.c"),
            Object(LinkedFor("mq-j"), "libc/k_tan.c"),
            Object(NotLinked, "libc/s_atan.c"),
            Object(LinkedFor("mq-j"), "libc/s_copysign.c"),
            Object(LinkedFor("mq-j"), "libc/s_cos.c"),
            Object(LinkedFor("mq-j"), "libc/s_floor.c"),
            Object(LinkedFor("mq-j"), "libc/s_frexp.c"),
            Object(NotLinked, "libc/s_ldexp.c"),
            Object(LinkedFor("mq-j"), "libc/s_modf.c"),
            Object(LinkedFor("mq-j"), "libc/s_sin.c"),
            Object(LinkedFor("mq-j"), "libc/s_tan.c"),
            Object(LinkedFor("mq-j"), "libc/w_acos.c"),
            Object(LinkedFor("mq-j"), "libc/w_atan2.c"),
            Object(LinkedFor("mq-j"), "libc/math_ppc.c"),
        ]
    ),
    GenericLib(
        "metrotrk",
        cflags_base,
        [
            Object(NotLinked, "metrotrk/mainloop.c"),
            Object(NotLinked, "metrotrk/nubevent.c"),
            Object(NotLinked, "metrotrk/nubinit.c"),
            Object(NotLinked, "metrotrk/msg.c"),
            Object(NotLinked, "metrotrk/msgbuf.c"),
            Object(NotLinked, "metrotrk/serpoll.c"),
            Object(NotLinked, "metrotrk/usr_put.c"),
            Object(NotLinked, "metrotrk/dispatch.c"),
            Object(NotLinked, "metrotrk/msghndlr.c"),
            Object(NotLinked, "metrotrk/support.c"),
            Object(NotLinked, "metrotrk/mutex_TRK.c"),
            Object(NotLinked, "metrotrk/notify.c"),
            Object(NotLinked, "metrotrk/flush_cache.c"),
            Object(NotLinked, "metrotrk/mem_TRK.c"),
            Object(NotLinked, "metrotrk/targimpl.c"),
            Object(NotLinked, "metrotrk/targsupp.s"),
            Object(NotLinked, "metrotrk/__exception.c"),
            Object(NotLinked, "metrotrk/dolphin_trk.c"),
            Object(NotLinked, "metrotrk/mpc_7xx_603e.c"),
            Object(NotLinked, "metrotrk/main_TRK.c"),
            Object(NotLinked, "metrotrk/dolphin_trk_glue.c"),
            Object(NotLinked, "metrotrk/targcont.c"),
            Object(NotLinked, "metrotrk/target_options.c"),
            Object(NotLinked, "metrotrk/mslsupp.c"),
        ]
    ),
    GenericLib(
        "debugger",
        cflags_base,
        [
            Object(LinkedFor("mq-j"), "debugger/AmcExi2Stubs.c"),
            Object(NotLinked, "debugger/DebuggerDriver.c"),
            Object(LinkedFor("mq-j"), "debugger/odenotstub.c"),
        ]
    ),
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Linked (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects

# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("menu", "Menu Code"),
    ProgressCategory("dolphin", "Dolphin SDK"),
    ProgressCategory("jsystem", "JSystem SDK"),
    ProgressCategory("runtime", "Runtime"),
    ProgressCategory("libc", "Libc"),
    ProgressCategory("metrotrk", "MetroTRK"),
    ProgressCategory("debugger", "Debugger Driver"),
]
config.progress_each_module = args.verbose

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress and write progress.json
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
