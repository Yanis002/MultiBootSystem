#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object NonMatching status.
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

### Script's arguments

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "--non-matching",
    action="store_true",
    help="create non-matching build for modding",
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
    "--progress-version",
    metavar="VERSION",
    help="version to print progress for",
)

args = parser.parse_args()

### Project configuration

config = ProjectConfig()
config.versions = [
    "mq-j",
]
config.default_version = "mq-j"
config.warn_missing_config = True
config.warn_missing_source = False
config.progress_all = False

config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.sjiswrap_path = args.sjiswrap
config.non_matching = args.non_matching

if not is_windows():
    config.wrapper = args.wrapper

if args.no_asm:
    config.asm_dir = None

### Tool versions

config.binutils_tag = "2.42-1"
config.compilers_tag = "20231018"
config.dtk_tag = "v0.8.3"
config.sjiswrap_tag = "v1.1.1"
config.wibo_tag = "0.6.11"
config.linker_version = "GC/1.3.2"

### Flags

config.asflags = [
    "-mgekko",
    "-I include",
    "-I libc",
    "-I libcpp",
]

config.ldflags = [
    "-fp hardware",
    "-nodefaults",
    "-warn off",
]

cflags_base = [
    "-Cpp_exceptions off",
    "-proc gekko",
    "-fp hardware",
    "-fp_contract on",
    "-enum int",
    "-align powerpc",
    "-nosyspath",
    "-RTTI on",
    "-str reuse",
    "-multibyte",
    "-O4,p",
    "-inline auto",
    "-nodefaults",
    "-msgstyle gcc",
    "-sym on",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-i include",
    "-i libc",
    "-i libcpp",
]

if config.non_matching:
    cflags_base.append("-DNON_MATCHING")

### Helper functions

def MenuLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": [*cflags_base, "-inline auto"],
        "host": False,
        "objects": objects,
    }

def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "host": False,
        "objects": objects,
    }

def JSystemLib(lib_name, objects):
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
        "host": True,
        "objects": objects,
    }

def GenericLib(lib_name: str, cflags: List[str], objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags,
        "host": False,
        "objects": objects,
    }

### Link order

# Not matching for any version
NonMatching = {}

# Matching for all versions
Matching = config.versions

# Matching for specific versions
def MatchingFor(*versions):
    return versions

config.libs = [
    MenuLib(
        "menu",
        [
            Object(NonMatching, "menu/main.cpp"),
            Object(NonMatching, "menu/dtk_stuff.cpp"),
            Object(MatchingFor("mq-j"), "menu/graphics.cpp"),
            Object(NonMatching, "menu/mixedcontroller.cpp"),
            Object(NonMatching, "menu/mtrand.cpp"),
            Object(MatchingFor("mq-j"), "menu/discerror.c"),
            Object(NonMatching, "menu/soundeffect.cpp"),
        ]
    ),
    DolphinLib(
        "exec",
        [
            Object(NonMatching, "dolphin/exec/exec.c"),
        ]
    ),
    DolphinLib(
        "base",
        [
            Object(MatchingFor("mq-j"), "dolphin/base/PPCArch.c"),
        ]
    ),
    DolphinLib(
        "os",
        [
            Object(MatchingFor("mq-j"), "dolphin/os/OS.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSAlarm.c"),
            Object(NonMatching, "dolphin/os/OSAlloc.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSArena.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSAudioSystem.c"),
            Object(NonMatching, "dolphin/os/OSCache.c"),
            Object(NonMatching, "dolphin/os/OSContext.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSError.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSFont.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSInterrupt.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSLink.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSMessage.c"),
            Object(NonMatching, "dolphin/os/OSMemory.c"),
            Object(NonMatching, "dolphin/os/OSMutex.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSReboot.c"),
            Object(NonMatching, "dolphin/os/OSReset.c"),
            Object(NonMatching, "dolphin/os/OSResetSW.c"),
            Object(NonMatching, "dolphin/os/OSRtc.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSSync.c"),
            Object(NonMatching, "dolphin/os/OSThread.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/OSTime.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/__start.c"),
            Object(MatchingFor("mq-j"), "dolphin/os/__ppc_eabi_init.c"),
        ]
    ),
    DolphinLib(
        "exi",
        [
            Object(MatchingFor("mq-j"), "dolphin/exi/EXIBios.c"),
            Object(MatchingFor("mq-j"), "dolphin/exi/EXIUart.c"),
        ]
    ),
    DolphinLib(
        "si",
        [
            Object(MatchingFor("mq-j"), "dolphin/si/SIBios.c"),
            Object(MatchingFor("mq-j"), "dolphin/si/SISamplingRate.c"),
        ]
    ),
    DolphinLib(
        "db",
        [
            Object(MatchingFor("mq-j"), "dolphin/db/db.c"),
        ]
    ),
    DolphinLib(
        "mtx",
        [
            Object(NonMatching, "dolphin/mtx/mtx.c"),
            Object(NonMatching, "dolphin/mtx/mtxvec.c"),
            Object(NonMatching, "dolphin/mtx/mtx44.c"),
            Object(NonMatching, "dolphin/mtx/vec.c"),
        ]
    ),
    DolphinLib(
        "dvd",
        [
            Object(MatchingFor("mq-j"), "dolphin/dvd/dvdlow.c"),
            Object(NonMatching, "dolphin/dvd/dvdfs.c"),
            Object(NonMatching, "dolphin/dvd/dvd.c"),
            Object(MatchingFor("mq-j"), "dolphin/dvd/dvdqueue.c"),
            Object(MatchingFor("mq-j"), "dolphin/dvd/dvderror.c"),
            Object(MatchingFor("mq-j"), "dolphin/dvd/dvdidutils.c"),
            Object(MatchingFor("mq-j"), "dolphin/dvd/dvdFatal.c"),
            Object(MatchingFor("mq-j"), "dolphin/dvd/fstload.c"),
        ]
    ),
    DolphinLib(
        "vi",
        [
            Object(NonMatching, "dolphin/vi/vi.c"),
        ]
    ),
    DolphinLib(
        "pad",
        [
            Object(MatchingFor("mq-j"), "dolphin/pad/Padclamp.c"),
            Object(MatchingFor("mq-j"), "dolphin/pad/Pad.c"),
        ]
    ),
    DolphinLib(
        "ai",
        [
            Object(NonMatching, "dolphin/ai/ai.c"),
        ]
    ),
    DolphinLib(
        "ar",
        [
            Object(NonMatching, "dolphin/ar/ar.c"),
            Object(NonMatching, "dolphin/ar/arq.c"),
        ]
    ),
    DolphinLib(
        "ax",
        [
            Object(NonMatching, "dolphin/ax/AX.c"),
            Object(NonMatching, "dolphin/ax/AXAlloc.c"),
            Object(NonMatching, "dolphin/ax/AXAux.c"),
            Object(NonMatching, "dolphin/ax/AXCL.c"),
            Object(NonMatching, "dolphin/ax/AXOut.c"),
            Object(NonMatching, "dolphin/ax/AXSPB.c"),
            Object(NonMatching, "dolphin/ax/AXVPB.c"),
            Object(NonMatching, "dolphin/ax/AXComp.c"),
            Object(NonMatching, "dolphin/ax/DSPCode.c"),
            Object(NonMatching, "dolphin/ax/AXProf.c"),
        ]
    ),
    DolphinLib(
        "mix",
        [
            Object(NonMatching, "dolphin/mix/mix.c"),
        ]
    ),
    DolphinLib(
        "sp",
        [
            Object(NonMatching, "dolphin/sp/sp.c"),
        ]
    ),
    DolphinLib(
        "am",
        [
            Object(NonMatching, "dolphin/am/am.c"),
        ]
    ),
    DolphinLib(
        "dsp",
        [
            Object(NonMatching, "dolphin/dsp/dsp.c"),
            Object(MatchingFor("mq-j"), "dolphin/dsp/dsp_debug.c"),
            Object(MatchingFor("mq-j"), "dolphin/dsp/dsp_task.c"),
        ]
    ),
    DolphinLib(
        "dtk",
        [
            Object(NonMatching, "dolphin/dtk/dtk.c"),
        ]
    ),
    DolphinLib(
        "card",
        [
            Object(MatchingFor("mq-j"), "dolphin/card/CARDBios.c"),
            Object(MatchingFor("mq-j"), "dolphin/card/CARDMount.c"),
        ]
    ),
    DolphinLib(
        "gx",
        [
            Object(NonMatching, "dolphin/gx/GXInit.c"),
            Object(NonMatching, "dolphin/gx/GXFifo.c"),
            Object(NonMatching, "dolphin/gx/GXAttr.c"),
            Object(NonMatching, "dolphin/gx/GXMisc.c"),
            Object(NonMatching, "dolphin/gx/GXGeometry.c"),
            Object(NonMatching, "dolphin/gx/GXFrameBuf.c"),
            Object(NonMatching, "dolphin/gx/GXLight.c"),
            Object(NonMatching, "dolphin/gx/GXTexture.c"),
            Object(NonMatching, "dolphin/gx/GXBump.c"),
            Object(NonMatching, "dolphin/gx/GXTev.c"),
            Object(NonMatching, "dolphin/gx/GXPixel.c"),
            Object(NonMatching, "dolphin/gx/GXStubs.c"),
            Object(NonMatching, "dolphin/gx/GXDisplayList.c"),
            Object(NonMatching, "dolphin/gx/GXTransform.c"),
            Object(NonMatching, "dolphin/gx/GXPerf.c"),
        ]
    ),
    DolphinLib(
        "gd",
        [
            Object(NonMatching, "dolphin/gd/GDBase.c"),
            Object(NonMatching, "dolphin/gd/GDGeometry.c"),
        ]
    ),
    JSystemLib(
        "JKernel",
        [
            Object(NonMatching, "JSystem/JKernel/JKRHeap.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRStdHeap.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRExpHeap.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRSolidHeap.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRDisposer.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRThread.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRAram.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRAramHeap.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRAramBlock.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRAramPiece.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRAramStream.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRFileLoader.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRFileFinder.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRArchivePub.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRArchivePri.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRMemArchive.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRAramArchive.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRDvdArchive.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRCompArchive.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRDvdFile.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRDvdRipper.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRDvdAramRipper.cpp"),
            Object(NonMatching, "JSystem/JKernel/JKRDecomp.cpp"),
        ]
    ),
    JSystemLib(
        "JSupport",
        [
            Object(NonMatching, "JSystem/JSupport/JSUList.cpp"),
            Object(NonMatching, "JSystem/JSupport/JSUInputStream.cpp"),
            Object(NonMatching, "JSystem/JSupport/JSUMemoryStream.cpp"),
            Object(NonMatching, "JSystem/JSupport/JSUFileStream.cpp"),
        ]
    ),
    JSystemLib(
        "JGadget",
        [
            Object(NonMatching, "JSystem/JGadget/linklist.cpp"),
        ]
    ),
    JSystemLib(
        "JUtility",
        [
            Object(NonMatching, "JSystem/JUtility/JUTCacheFont.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTResource.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTTexture.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTPalette.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTNameTab.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTRect.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTGraphFifo.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTFont.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTResFont.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTDbPrint.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTGamePad.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTFontData_Ascfont_fix12.s"),
            Object(NonMatching, "JSystem/JUtility/JUTException.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTDirectPrint.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTAssert.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTVideo.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTXfb.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTFader.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTProcBar.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTConsole.cpp"),
            Object(NonMatching, "JSystem/JUtility/JUTDirectFile.cpp"),
        ]
    ),
    JSystemLib(
        "J2DGraph",
        [
            Object(NonMatching, "JSystem/J2DGraph/J2DGrafContext.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DOrthoGraph.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DPrint.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DPane.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DScreen.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DWindow.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DPicture.cpp"),
            Object(NonMatching, "JSystem/J2DGraph/J2DTextBox.cpp"),
        ]
    ),
    JSystemLib(
        "JRenderer",
        [
            Object(NonMatching, "JSystem/JRenderer/JRenderer.cpp"),
        ]
    ),
    JSystemLib(
        "J3DGraphBase",
        [
            Object(NonMatching, "JSystem/J3DGraphBase/J3DGD.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DSys.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DVertex.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DTransform.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DPacket.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DShapeMtx.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DShape.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DMaterial.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DMatBlock.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DTevs.cpp"),
            Object(NonMatching, "JSystem/J3DGraphBase/J3DDrawBuffer.cpp"),
        ]
    ),
    JSystemLib(
        "J3DGraphAnimator",
        [
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DModelData.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DModel.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DAnimation.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DMaterialAnm.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DCluster.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DJoint.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DNode.cpp"),
            Object(NonMatching, "JSystem/J3DGraphAnimator/J3DMaterialAttach.cpp"),
        ]
    ),
    JSystemLib(
        "J3DGraphLoader",
        [
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DMaterialFactory.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DMaterialFactory_v21.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DClusterLoader.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DModelLoader.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DModelLoaderCalcSize.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DJointFactory.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DShapeFactory.cpp"),
            Object(NonMatching, "JSystem/J3DGraphLoader/J3DAnmLoader.cpp"),
        ]
    ),
    JSystemLib(
        "JMath",
        [
            Object(NonMatching, "JSystem/JMath/JMath.cpp"),
            Object(NonMatching, "JSystem/JMath/random.cpp"),
        ]
    ),
    JSystemLib(
        "JFramework",
        [
            Object(NonMatching, "JSystem/JFramework/JFWSystem.cpp"),
            Object(NonMatching, "JSystem/JFramework/JFWDisplay.cpp"),
        ]
    ),
    JSystemLib(
        "JPALoader",
        [
            Object(NonMatching, "JSystem/JPALoader/JPABaseShape.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPAExtraShape.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPASweepShape.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPAExTexShape.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPADynamicsBlock.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPAFieldBlock.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPAKeyBlock.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPATexture.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPAResourceManager.cpp"),
            Object(NonMatching, "JSystem/JPALoader/JPAEmitterLoader.cpp"),
        ]
    ),
    JSystemLib(
        "JPABase",
        [
            Object(NonMatching, "JSystem/JPABase/JPAMath.cpp"),
            Object(NonMatching, "JSystem/JPABase/JPAField.cpp"),
            Object(NonMatching, "JSystem/JPABase/JPAEmitter.cpp"),
            Object(NonMatching, "JSystem/JPABase/JPAParticle.cpp"),
            Object(NonMatching, "JSystem/JPABase/JPAEmitterManager.cpp"),
        ]
    ),
    JSystemLib(
        "JPADraw",
        [
            Object(NonMatching, "JSystem/JPADraw/JPADrawVisitor.cpp"),
            Object(NonMatching, "JSystem/JPADraw/JPADraw.cpp"),
            Object(NonMatching, "JSystem/JPADraw/JPADrawSetupTev.cpp"),
        ]
    ),
    GenericLib(
        "runtime",
        [*cflags_base, "-use_lmw_stmw on", "-str reuse,readonly", "-inline auto,deferred"],
        [
            Object(MatchingFor("mq-j"), "runtime/__va_arg.c"),
            Object(MatchingFor("mq-j"), "runtime/global_destructor_chain.c"),
            Object(NonMatching, "runtime/NMWException.cp"),
            Object(MatchingFor("mq-j"), "runtime/ptmf.c"),
            Object(MatchingFor("mq-j"), "runtime/runtime.c"),
            Object(MatchingFor("mq-j"), "runtime/__init_cpp_exceptions.cpp"),
            Object(MatchingFor("mq-j"), "runtime/Gecko_ExceptionPPC.cp"),
            Object(MatchingFor("mq-j"), "runtime/GCN_mem_alloc.c"),
        ]
    ),
    GenericLib(
        "libc",
        [*cflags_base, "-use_lmw_stmw on", "-str reuse,pool,readonly", "-inline deferred"],
        [
            Object(MatchingFor("mq-j"), "libc/abort_exit.c"),
            Object(NonMatching, "libc/alloc.c"),
            Object(MatchingFor("mq-j"), "libc/errno.c"),
            Object(MatchingFor("mq-j"), "libc/ansi_files.c"),
            Object(NonMatching, "libc/ansi_fp.c"),
            Object(MatchingFor("mq-j"), "libc/arith.c"),
            Object(MatchingFor("mq-j"), "libc/buffer_io.c"),
            Object(MatchingFor("mq-j"), "libc/ctype.c"),
            Object(MatchingFor("mq-j"), "libc/direct_io.c"),
            Object(MatchingFor("mq-j"), "libc/file_io.c"),
            Object(MatchingFor("mq-j"), "libc/file_pos.c"),
            Object(MatchingFor("mq-j"), "libc/mbstring.c"),
            Object(MatchingFor("mq-j"), "libc/mem.c"),
            Object(MatchingFor("mq-j"), "libc/mem_funcs.c"),
            Object(MatchingFor("mq-j"), "libc/misc_io.c"),
            Object(MatchingFor("mq-j"), "libc/printf.c"),
            Object(MatchingFor("mq-j"), "libc/float.c"),
            Object(MatchingFor("mq-j"), "libc/scanf.c"),
            Object(MatchingFor("mq-j"), "libc/string.c"),
            Object(MatchingFor("mq-j"), "libc/strtoul.c"),
            Object(MatchingFor("mq-j"), "libc/uart_console_io.c"),
            Object(MatchingFor("mq-j"), "libc/wchar_io.c"),
            Object(MatchingFor("mq-j"), "libc/e_acos.c"),
            Object(MatchingFor("mq-j"), "libc/e_atan2.c"),
            Object(MatchingFor("mq-j"), "libc/e_rem_pio2.c"),
            Object(MatchingFor("mq-j"), "libc/k_cos.c"),
            Object(MatchingFor("mq-j"), "libc/k_rem_pio2.c"),
            Object(MatchingFor("mq-j"), "libc/k_sin.c"),
            Object(MatchingFor("mq-j"), "libc/k_tan.c"),
            Object(NonMatching, "libc/s_atan.c"),
            Object(MatchingFor("mq-j"), "libc/s_copysign.c"),
            Object(MatchingFor("mq-j"), "libc/s_cos.c"),
            Object(MatchingFor("mq-j"), "libc/s_floor.c"),
            Object(MatchingFor("mq-j"), "libc/s_frexp.c"),
            Object(NonMatching, "libc/s_ldexp.c"),
            Object(MatchingFor("mq-j"), "libc/s_modf.c"),
            Object(NonMatching, "libc/s_sin.c"),
            Object(NonMatching, "libc/s_tan.c"),
            Object(NonMatching, "libc/w_acos.c"),
            Object(NonMatching, "libc/w_atan2.c"),
            Object(MatchingFor("mq-j"), "libc/math_ppc.c"),
        ]
    ),
    GenericLib(
        "metrotrk",
        cflags_base,
        [
            Object(NonMatching, "metrotrk/mainloop.c"),
            Object(NonMatching, "metrotrk/nubevent.c"),
            Object(NonMatching, "metrotrk/nubinit.c"),
            Object(NonMatching, "metrotrk/msg.c"),
            Object(NonMatching, "metrotrk/msgbuf.c"),
            Object(NonMatching, "metrotrk/serpoll.c"),
            Object(NonMatching, "metrotrk/usr_put.c"),
            Object(NonMatching, "metrotrk/dispatch.c"),
            Object(NonMatching, "metrotrk/msghndlr.c"),
            Object(NonMatching, "metrotrk/support.c"),
            Object(NonMatching, "metrotrk/mutex_TRK.c"),
            Object(NonMatching, "metrotrk/notify.c"),
            Object(NonMatching, "metrotrk/flush_cache.c"),
            Object(NonMatching, "metrotrk/mem_TRK.c"),
            Object(NonMatching, "metrotrk/targimpl.c"),
            Object(NonMatching, "metrotrk/targsupp.s"),
            Object(NonMatching, "metrotrk/dolphin_trk.c"),
            Object(NonMatching, "metrotrk/mpc_7xx_603e.c"),
            Object(NonMatching, "metrotrk/main_TRK.c"),
            Object(NonMatching, "metrotrk/dolphin_trk_glue.c"),
            Object(NonMatching, "metrotrk/targcont.c"),
            Object(NonMatching, "metrotrk/target_options.c"),
            Object(NonMatching, "metrotrk/mslsupp.c"),
        ]
    ),
    GenericLib(
        "debugger",
        cflags_base,
        [
            Object(MatchingFor("mq-j"), "debugger/AmcExi2Stubs.c"),
            Object(NonMatching, "debugger/DebuggerDriver.c"),
            Object(MatchingFor("mq-j"), "debugger/odenotstub.c"),
        ]
    ),
]

### Execute mode

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress and write progress.json
    calculate_progress(config, args.progress_version)
else:
    sys.exit("Unknown mode: " + args.mode)
