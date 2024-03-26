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
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "D43J01",  # 0
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
    "--version",
    choices=VERSIONS,
    type=str.upper,
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
    default=True,
)
parser.add_argument(
    "--no-asm",
    action="store_true",
    help="don't incorporate .s files from asm directory",
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
args = parser.parse_args()

config = ProjectConfig()
config.version = args.version
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.debug = args.debug
config.generate_map = args.map
config.sjiswrap_path = args.sjiswrap
if not is_windows():
    config.wrapper = args.wrapper
if args.no_asm:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-1"
config.compilers_tag = "20231018"
config.dtk_tag = "v0.7.5"
config.sjiswrap_tag = "v1.1.1"
config.wibo_tag = "0.6.11"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym version={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
    "-warn off",
    # "-listclosure", # Uncomment for Wii linkers
]

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
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DVERSION={version_num}",
]

# Debug flags
if config.debug:
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    # "-gccinc",
    "-common off",
    "-inline auto",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/1.1"


# Helper function for libraries sharing the same informations
def GenericLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_runtime,
        "host": False,
        "objects": objects,
    }


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "host": False,
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "host": True,
        "objects": objects,
    }


Matching = True
NonMatching = False

config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "MultiBootSystem",
        "mw_version": "GC/1.1",
        "cflags": cflags_runtime,
        "host": False,
        "objects": [
            Object(NonMatching, "main.c"),
            Object(NonMatching, "dtk_stuff.c"),
            Object(NonMatching, "graphics.c"),
            Object(NonMatching, "mixedcontroller.c"),
            Object(NonMatching, "mtrand.c"),
            Object(Matching, "discerror.c"),
            Object(NonMatching, "soundeffect.c"),
        ]
    },
    DolphinLib(
        "exec.a",
        [
            Object(NonMatching, "exec.a/exec.c"),
        ]
    ),
    DolphinLib(
        "base.a",
        [
            Object(NonMatching, "base.a/PPCArch.c"),
        ]
    ),
    DolphinLib(
        "os.a",
        [
            Object(NonMatching, "os.a/OS.c"),
            Object(NonMatching, "os.a/OSAlarm.c"),
            Object(NonMatching, "os.a/OSAlloc.c"),
            Object(NonMatching, "os.a/OSArena.c"),
            Object(NonMatching, "os.a/OSAudioSystem.c"),
            Object(NonMatching, "os.a/OSCache.c"),
            Object(NonMatching, "os.a/OSContext.c"),
            Object(NonMatching, "os.a/OSError.c"),
            Object(NonMatching, "os.a/OSFont.c"),
            Object(NonMatching, "os.a/OSInterrupt.c"),
            Object(NonMatching, "os.a/OSLink.c"),
            Object(NonMatching, "os.a/OSMessage.c"),
            Object(NonMatching, "os.a/OSMemory.c"),
            Object(NonMatching, "os.a/OSMutex.c"),
            Object(NonMatching, "os.a/OSReboot.c"),
            Object(NonMatching, "os.a/OSReset.c"),
            Object(NonMatching, "os.a/OSResetSW.c"),
            Object(NonMatching, "os.a/OSRtc.c"),
            Object(NonMatching, "os.a/OSSync.c"),
            Object(NonMatching, "os.a/OSThread.c"),
            Object(NonMatching, "os.a/OSTime.c"),
            Object(NonMatching, "os.a/__ppc_eabi_init.cpp"),
        ]
    ),
    DolphinLib(
        "exi.a",
        [
            Object(NonMatching, "exi.a/EXIBios.c"),
            Object(NonMatching, "exi.a/EXIUart.c"),
        ]
    ),
    DolphinLib(
        "si.a",
        [
            Object(NonMatching, "si.a/SIBios.c"),
            Object(NonMatching, "si.a/SISamplingRate.c"),
        ]
    ),
    DolphinLib(
        "db.a",
        [
            Object(NonMatching, "db.a/db.c"),
        ]
    ),
    DolphinLib(
        "mtx.a",
        [
            Object(NonMatching, "mtx.a/mtx.c"),
            Object(NonMatching, "mtx.a/mtxvec.c"),
            Object(NonMatching, "mtx.a/mtx44.c"),
            Object(NonMatching, "mtx.a/vec.c"),
        ]
    ),
    DolphinLib(
        "dvd.a",
        [
            Object(NonMatching, "dvd.a/dvdlow.c"),
            Object(NonMatching, "dvd.a/dvdfs.c"),
            Object(NonMatching, "dvd.a/dvd.c"),
            Object(NonMatching, "dvd.a/dvdqueue.c"),
            Object(NonMatching, "dvd.a/dvderror.c"),
            Object(NonMatching, "dvd.a/dvdidutils.c"),
            Object(NonMatching, "dvd.a/dvdFatal.c"),
            Object(NonMatching, "dvd.a/fstload.c"),
        ]
    ),
    DolphinLib(
        "vi.a",
        [
            Object(NonMatching, "vi.a/vi.c"),
        ]
    ),
    DolphinLib(
        "pad.a",
        [
            Object(NonMatching, "pad.a/Padclamp.c"),
            Object(NonMatching, "pad.a/Pad.c"),
        ]
    ),
    DolphinLib(
        "ai.a",
        [
            Object(NonMatching, "ai.a/ai.c"),
        ]
    ),
    DolphinLib(
        "ar.a",
        [
            Object(NonMatching, "ar.a/ar.c"),
            Object(NonMatching, "ar.a/arq.c"),
        ]
    ),
    DolphinLib(
        "ax.a",
        [
            Object(NonMatching, "ax.a/AX.c"),
            Object(NonMatching, "ax.a/AXAlloc.c"),
            Object(NonMatching, "ax.a/AXAux.c"),
            Object(NonMatching, "ax.a/AXCL.c"),
            Object(NonMatching, "ax.a/AXOut.c"),
            Object(NonMatching, "ax.a/AXSPB.c"),
            Object(NonMatching, "ax.a/AXVPB.c"),
            Object(NonMatching, "ax.a/AXComp.c"),
            Object(NonMatching, "ax.a/DSPCode.c"),
            Object(NonMatching, "ax.a/AXProf.c"),
        ]
    ),
    DolphinLib(
        "mix.a",
        [
            Object(NonMatching, "mix.a/mix.c"),
        ]
    ),
    DolphinLib(
        "sp.a",
        [
            Object(NonMatching, "sp.a/sp.c"),
        ]
    ),
    DolphinLib(
        "am.a",
        [
            Object(NonMatching, "am.a/am.c"),
        ]
    ),
    DolphinLib(
        "dsp.a",
        [
            Object(NonMatching, "dsp.a/dsp.c"),
            Object(NonMatching, "dsp.a/dsp_debug.c"),
            Object(NonMatching, "dsp.a/dsp_task.c"),
        ]
    ),
    DolphinLib(
        "dtk.a",
        [
            Object(NonMatching, "dtk.a/dtk.c"),
        ]
    ),
    DolphinLib(
        "card.a",
        [
            Object(NonMatching, "card.a/CARDBios.c"),
            Object(NonMatching, "card.a/CARDMount.c"),
        ]
    ),
    DolphinLib(
        "gx.a",
        [
            Object(NonMatching, "gx.a/GXInit.c"),
            Object(NonMatching, "gx.a/GXFifo.c"),
            Object(NonMatching, "gx.a/GXAttr.c"),
            Object(NonMatching, "gx.a/GXMisc.c"),
            Object(NonMatching, "gx.a/GXGeometry.c"),
            Object(NonMatching, "gx.a/GXFrameBuf.c"),
            Object(NonMatching, "gx.a/GXLight.c"),
            Object(NonMatching, "gx.a/GXTexture.c"),
            Object(NonMatching, "gx.a/GXBump.c"),
            Object(NonMatching, "gx.a/GXTev.c"),
            Object(NonMatching, "gx.a/GXPixel.c"),
            Object(NonMatching, "gx.a/GXStubs.c"),
            Object(NonMatching, "gx.a/GXDisplayList.c"),
            Object(NonMatching, "gx.a/GXTransform.c"),
            Object(NonMatching, "gx.a/GXPerf.c"),
        ]
    ),
    DolphinLib(
        "gd.a",
        [
            Object(NonMatching, "gd.a/GDBase.c"),
            Object(NonMatching, "gd.a/GDGeometry.c"),
        ]
    ),
    GenericLib(
        "JKernel.a",
        [
            Object(NonMatching, "JKernel.a/JKRHeap.cpp"),
            Object(NonMatching, "JKernel.a/JKRStdHeap.cpp"),
            Object(NonMatching, "JKernel.a/JKRExpHeap.cpp"),
            Object(NonMatching, "JKernel.a/JKRSolidHeap.cpp"),
            Object(NonMatching, "JKernel.a/JKRDisposer.cpp"),
            Object(NonMatching, "JKernel.a/JKRThread.cpp"),
            Object(NonMatching, "JKernel.a/JKRAram.cpp"),
            Object(NonMatching, "JKernel.a/JKRAramHeap.cpp"),
            Object(NonMatching, "JKernel.a/JKRAramBlock.cpp"),
            Object(NonMatching, "JKernel.a/JKRAramPiece.cpp"),
            Object(NonMatching, "JKernel.a/JKRAramStream.cpp"),
            Object(NonMatching, "JKernel.a/JKRFileLoader.cpp"),
            Object(NonMatching, "JKernel.a/JKRFileFinder.cpp"),
            Object(NonMatching, "JKernel.a/JKRArchivePub.cpp"),
            Object(NonMatching, "JKernel.a/JKRArchivePri.cpp"),
            Object(NonMatching, "JKernel.a/JKRMemArchive.cpp"),
            Object(NonMatching, "JKernel.a/JKRAramArchive.cpp"),
            Object(NonMatching, "JKernel.a/JKRDvdArchive.cpp"),
            Object(NonMatching, "JKernel.a/JKRCompArchive.cpp"),
            Object(NonMatching, "JKernel.a/JKRDvdFile.cpp"),
            Object(NonMatching, "JKernel.a/JKRDvdRipper.cpp"),
            Object(NonMatching, "JKernel.a/JKRDvdAramRipper.cpp"),
            Object(NonMatching, "JKernel.a/JKRDecomp.cpp"),
        ]
    ),
    GenericLib(
        "JSupport.a",
        [
            Object(NonMatching, "JSupport.a/JSUList.cpp"),
            Object(NonMatching, "JSupport.a/JSUInputStream.cpp"),
            Object(NonMatching, "JSupport.a/JSUMemoryStream.cpp"),
            Object(NonMatching, "JSupport.a/JSUFileStream.cpp"),
        ]
    ),
    GenericLib(
        "JGadget.a",
        [
            Object(NonMatching, "JGadget.a/linklist.cpp"),
        ]
    ),
    GenericLib(
        "JUtility.a",
        [
            Object(NonMatching, "JUtility.a/JUTCacheFont.cpp"),
            Object(NonMatching, "JUtility.a/JUTResource.cpp"),
            Object(NonMatching, "JUtility.a/JUTTexture.cpp"),
            Object(NonMatching, "JUtility.a/JUTPalette.cpp"),
            Object(NonMatching, "JUtility.a/JUTNameTab.cpp"),
            Object(NonMatching, "JUtility.a/JUTRect.cpp"),
            Object(NonMatching, "JUtility.a/JUTGraphFifo.cpp"),
            Object(NonMatching, "JUtility.a/JUTFont.cpp"),
            Object(NonMatching, "JUtility.a/JUTResFont.cpp"),
            Object(NonMatching, "JUtility.a/JUTDbPrint.cpp"),
            Object(NonMatching, "JUtility.a/JUTGamePad.cpp"),
            Object(NonMatching, "JUtility.a/JUTFontData_Ascfont_fix12.s"),
            Object(NonMatching, "JUtility.a/JUTException.cpp"),
            Object(NonMatching, "JUtility.a/JUTDirectPrint.cpp"),
            Object(NonMatching, "JUtility.a/JUTAssert.cpp"),
            Object(NonMatching, "JUtility.a/JUTVideo.cpp"),
            Object(NonMatching, "JUtility.a/JUTXfb.cpp"),
            Object(NonMatching, "JUtility.a/JUTFader.cpp"),
            Object(NonMatching, "JUtility.a/JUTProcBar.cpp"),
            Object(NonMatching, "JUtility.a/JUTConsole.cpp"),
            Object(NonMatching, "JUtility.a/JUTDirectFile.cpp"),
        ]
    ),
    GenericLib(
        "J2DGraph.a",
        [
            Object(NonMatching, "J2DGraph.a/J2DGrafContext.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DOrthoGraph.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DPrint.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DPane.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DScreen.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DWindow.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DPicture.cpp"),
            Object(NonMatching, "J2DGraph.a/J2DTextBox.cpp"),
        ]
    ),
    GenericLib(
        "JRenderer.a",
        [
            Object(NonMatching, "JRenderer.a/JRenderer.cpp"),
        ]
    ),
    GenericLib(
        "J3DGraphBase.a",
        [
            Object(NonMatching, "J3DGraphBase.a/J3DGD.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DSys.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DVertex.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DTransform.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DPacket.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DShapeMtx.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DShape.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DMaterial.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DMatBlock.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DTevs.cpp"),
            Object(NonMatching, "J3DGraphBase.a/J3DDrawBuffer.cpp"),
        ]
    ),
    GenericLib(
        "J3DGraphAnimator.a",
        [
            Object(NonMatching, "J3DGraphAnimator.a/J3DModelData.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DModel.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DAnimation.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DMaterialAnm.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DCluster.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DJoint.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DNode.cpp"),
            Object(NonMatching, "J3DGraphAnimator.a/J3DMaterialAttach.cpp"),
        ]
    ),
    GenericLib(
        "J3DGraphLoader.a",
        [
            Object(NonMatching, "J3DGraphLoader.a/J3DMaterialFactory.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DMaterialFactory_v21.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DClusterLoader.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DModelLoader.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DModelLoaderCalcSize.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DJointFactory.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DShapeFactory.cpp"),
            Object(NonMatching, "J3DGraphLoader.a/J3DAnmLoader.cpp"),
        ]
    ),
    GenericLib(
        "JMath.a",
        [
            Object(NonMatching, "JMath.a/JMath.cpp"),
            Object(NonMatching, "JMath.a/random.cpp"),
        ]
    ),
    GenericLib(
        "JFramework.a",
        [
            Object(NonMatching, "JFramework.a/JFWSystem.cpp"),
            Object(NonMatching, "JFramework.a/JFWDisplay.cpp"),
        ]
    ),
    GenericLib(
        "JPALoader.a",
        [
            Object(NonMatching, "JPALoader.a/JPABaseShape.cpp"),
            Object(NonMatching, "JPALoader.a/JPAExtraShape.cpp"),
            Object(NonMatching, "JPALoader.a/JPASweepShape.cpp"),
            Object(NonMatching, "JPALoader.a/JPAExTexShape.cpp"),
            Object(NonMatching, "JPALoader.a/JPADynamicsBlock.cpp"),
            Object(NonMatching, "JPALoader.a/JPAFieldBlock.cpp"),
            Object(NonMatching, "JPALoader.a/JPAKeyBlock.cpp"),
            Object(NonMatching, "JPALoader.a/JPATexture.cpp"),
            Object(NonMatching, "JPALoader.a/JPAResourceManager.cpp"),
            Object(NonMatching, "JPALoader.a/JPAEmitterLoader.cpp"),
        ]
    ),
    GenericLib(
        "JPABase.a",
        [
            Object(NonMatching, "JPABase.a/JPAMath.cpp"),
            Object(NonMatching, "JPABase.a/JPAField.cpp"),
            Object(NonMatching, "JPABase.a/JPAEmitter.cpp"),
            Object(NonMatching, "JPABase.a/JPAParticle.cpp"),
            Object(NonMatching, "JPABase.a/JPAEmitterManager.cpp"),
        ]
    ),
    GenericLib(
        "JPADraw.a",
        [
            Object(NonMatching, "JPADraw.a/JPADrawVisitor.cpp"),
            Object(NonMatching, "JPADraw.a/JPADraw.cpp"),
            Object(NonMatching, "JPADraw.a/JPADrawSetupTev.cpp"),
        ]
    ),
    GenericLib(
        "Runtime.PPCEABI.H.a",
        [
            Object(NonMatching, "Runtime.PPCEABI.H.a/__va_arg.c"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/global_destructor_chain.c"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/NMWException.cp"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/ptmf.c"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/runtime.c"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/__init_cpp_exceptions.cpp"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/Gecko_ExceptionPPC.cp"),
            Object(NonMatching, "Runtime.PPCEABI.H.a/GCN_mem_alloc.c"),
        ]
    ),
    GenericLib(
        "MSL_C.PPCEABI.bare.H.a",
        [
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/abort_exit.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/alloc.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/errno.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/ansi_files.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/ansi_fp.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/arith.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/buffer_io.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/ctype.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/direct_io.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/file_io.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/FILE_POS.C"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/mbstring.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/mem.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/mem_funcs.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/misc_io.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/printf.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/float.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/scanf.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/string.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/strtoul.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/uart_console_io.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/wchar_io.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/e_acos.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/e_atan2.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/e_rem_pio2.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/k_cos.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/k_rem_pio2.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/k_sin.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/k_tan.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_atan.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_copysign.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_cos.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_floor.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_frexp.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_ldexp.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_modf.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_sin.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/s_tan.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/w_acos.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/w_atan2.c"),
            Object(NonMatching, "MSL_C.PPCEABI.bare.H.a/math_ppc.c"),
        ]
    ),
    GenericLib(
        "TRK_MINNOW_DOLPHIN.a",
        [
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/mainloop.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/nubevent.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/nubinit.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/msg.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/msgbuf.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/serpoll.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/usr_put.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/dispatch.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/msghndlr.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/support.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/mutex_TRK.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/notify.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/flush_cache.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/mem_TRK.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/targimpl.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/targsupp.s"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/dolphin_trk.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/mpc_7xx_603e.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/main_TRK.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/dolphin_trk_glue.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/targcont.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/target_options.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN.a/mslsupp.c"),
        ]
    ),
    GenericLib(
        "amcstubs.a",
        [
            Object(NonMatching, "amcstubs.a/AmcExi2Stubs.c"),
        ]
    ),
    GenericLib(
        "OdemuExi2.a",
        [
            Object(NonMatching, "OdemuExi2.a/DebuggerDriver.c"),
        ]
    ),
    GenericLib(
        "odenotstub.a",
        [
            Object(NonMatching, "odenotstub.a/odenotstub.c"),
        ]
    ),
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress and write progress.json
    config.progress_each_module = args.verbose
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
