#ifndef J3DVERTEX_H
#define J3DVERTEX_H

#include "dolphin/gx/GXAttr.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"

typedef struct _GXColor GXColor;
class J3DModel;
class J3DAnmVtxColor;
class J3DVertexBuffer;

struct J3DVtxColorCalc {
    void calc(J3DModel*);
    virtual void calc(J3DVertexBuffer*);

    /* 0x0 */ void* vtable;  // inlined vtable?
    /* 0x4 */ u32 mFlags;
    /* 0x8 */ J3DAnmVtxColor* mpVtxColor;
};

class J3DVertexData {
public:
    J3DVertexData();
    ~J3DVertexData();

    inline void clear() { mPacketNum = 0; }

    void* getVtxPosArray() { return mVtxPosArray; }
    void* getVtxNrmArray() { return mVtxNrmArray; }
    GXColor* getVtxColorArray(u8 idx) const { return mVtxColorArray[idx]; }
    void* getVtxTexCoordArray(u8 idx) { return mVtxTexCoordArray[idx]; }
    void* getVtxNBTArray() { return mVtxNBTArray; }
    u32 getNrmNum() const { return mNrmNum; }
    u32 getVtxNum() const { return mVtxNum; }
    GXVtxAttrFmtList* getVtxAttrFmtList() { return mVtxAttrFmtList; }

    GXCompType getVtxPosType() const { return mVtxPosType; }
    void setVtxPosType(GXCompType type) { mVtxPosType = type; }
    u8 getVtxPosFrac() const { return mVtxPosFrac; }
    void setVtxPosFrac(u8 frac) { mVtxPosFrac = frac; }
    GXCompType getVtxNrmType() const { return mVtxNrmType; }
    void setVtxNrmType(GXCompType type) { mVtxNrmType = type; }
    u8 getVtxNrmFrac() const { return mVtxNrmFrac; }
    void setVtxNrmFrac(u8 frac) { mVtxNrmFrac = frac; }

private:
    friend class J3DModelLoader;

    /* 0x00 */ u32 mVtxNum;
    /* 0x04 */ u32 mNrmNum;
    /* 0x08 */ u32 mColNum;
    /* 0x0C */ u32 mTexCoordNum;
    /* 0x10 */ GXVtxAttrFmtList* mVtxAttrFmtList;
    /* 0x14 */ void* mVtxPosArray;
    /* 0x18 */ void* mVtxNrmArray;
    /* 0x1C */ void* mVtxNBTArray;
    /* 0x20 */ GXColor* mVtxColorArray[2];
    /* 0x28 */ void* mVtxTexCoordArray[8];
    /* 0x48 */ u8 mVtxPosFrac;
    /* 0x4C */ GXCompType mVtxPosType;
    /* 0x50 */ u8 mVtxNrmFrac;
    /* 0x54 */ GXCompType mVtxNrmType;
    /* 0x58 */ u32 mPacketNum;
};

class J3DVertexBuffer {
public:
    J3DVertexBuffer() { init(); }

    void setVertexData(J3DVertexData*);
    void init();
    ~J3DVertexBuffer();
    void setArray() const;
    s32 copyLocalVtxPosArray(u32);
    s32 copyLocalVtxNrmArray(u32);
    s32 copyLocalVtxArray(u32);
    s32 allocTransformedVtxPosArray();
    s32 allocTransformedVtxNrmArray();

    J3DVertexData* getVertexData() { return mVtxData; }

    void setCurrentVtxPos(void* pVtxPos) { mCurrentVtxPos = pVtxPos; }
    void* getCurrentVtxPos() { return mCurrentVtxPos; }

    void setCurrentVtxNrm(void* pVtxNrm) { mCurrentVtxNrm = pVtxNrm; }
    void* getCurrentVtxNrm() { return mCurrentVtxNrm; }

    void setCurrentVtxCol(GXColor* pVtxCol) { mCurrentVtxCol = pVtxCol; }

    void frameInit() {
        setCurrentVtxPos(mVtxPosArray[0]);
        setCurrentVtxNrm(mVtxNrmArray[0]);
        setCurrentVtxCol(mVtxColArray[0]);
    }

    void* getTransformedVtxPos(int idx) { return mTransformedVtxPosArray[idx]; }
    void* getTransformedVtxNrm(int idx) { return mTransformedVtxNrmArray[idx]; }
    void* getVtxPosArrayPointer(int idx) { return mVtxPosArray[idx]; }
    void* getVtxNrmArrayPointer(int idx) { return mVtxNrmArray[idx]; }

    void swapVtxPosArrayPointer() {
        void* tmp = mVtxPosArray[0];
        mVtxPosArray[0] = mVtxPosArray[1];
        mVtxPosArray[1] = tmp;
    }

    void swapVtxNrmArrayPointer() {
        void* tmp = mVtxNrmArray[0];
        mVtxNrmArray[0] = mVtxNrmArray[1];
        mVtxNrmArray[1] = tmp;
    }

    void swapTransformedVtxPos() {
        void* tmp = mTransformedVtxPosArray[0];
        mTransformedVtxPosArray[0] = mTransformedVtxPosArray[1];
        mTransformedVtxPosArray[1] = tmp;
    }

    void swapTransformedVtxNrm() {
        void* tmp = mTransformedVtxNrmArray[0];
        mTransformedVtxNrmArray[0] = mTransformedVtxNrmArray[1];
        mTransformedVtxNrmArray[1] = tmp;
    }

private:
    /* 0x00 */ J3DVertexData* mVtxData;
    /* 0x04 */ void* mVtxPosArray[2];
    /* 0x0C */ void* mVtxNrmArray[2];
    /* 0x14 */ GXColor* mVtxColArray[2];
    /* 0x1C */ void* mTransformedVtxPosArray[2];
    /* 0x24 */ void* mTransformedVtxNrmArray[2];
    /* 0x2C */ void* mCurrentVtxPos;
    /* 0x30 */ void* mCurrentVtxNrm;
    /* 0x34 */ GXColor* mCurrentVtxCol;
};  // Size: 0x38

struct VertexNormal {
    Vec data;
};

#endif /* J3DVERTEX_H */
