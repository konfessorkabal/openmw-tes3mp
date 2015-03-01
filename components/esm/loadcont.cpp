#include "loadcont.hpp"

#include "esmreader.hpp"
#include "esmwriter.hpp"
#include "defs.hpp"

namespace ESM
{

    void InventoryList::add(ESMReader &esm)
    {
        ContItem ci;
        esm.getHT(ci, 36);
        mList.push_back(ci);
    }

    void InventoryList::save(ESMWriter &esm) const
    {
        for (std::vector<ContItem>::const_iterator it = mList.begin(); it != mList.end(); ++it)
        {
            esm.writeHNT("NPCO", *it, 36);
        }
    }

    unsigned int Container::sRecordId = REC_CONT;

    void Container::load(ESMReader &esm)
    {
        mInventory.mList.clear();
        bool hasWeight = false;
        bool hasFlags = false;
        while (esm.hasMoreSubs())
        {
            esm.getSubName();
            uint32_t name = esm.retSubName().val;
            switch (name)
            {
                case ESM::FourCC<'M','O','D','L'>::value:
                    mModel = esm.getHString();
                    break;
                case ESM::FourCC<'F','N','A','M'>::value:
                    mName = esm.getHString();
                    break;
                case ESM::FourCC<'C','N','D','T'>::value:
                    esm.getHT(mWeight, 4);
                    hasWeight = true;
                    break;
                case ESM::FourCC<'F','L','A','G'>::value:
                    esm.getHT(mFlags, 4);
                    if (mFlags & 0xf4)
                        esm.fail("Unknown flags");
                    if (!(mFlags & 0x8))
                        esm.fail("Flag 8 not set");
                    hasFlags = true;
                    break;
                case ESM::FourCC<'S','C','R','I'>::value:
                    mScript = esm.getHString();
                    break;
                case ESM::FourCC<'N','P','C','O'>::value:
                    mInventory.add(esm);
                    break;
                default:
                    esm.fail("Unknown subrecord");
            }
        }
        if (!hasWeight)
            esm.fail("Missing CNDT subrecord");
        if (!hasFlags)
            esm.fail("Missing FLAG subrecord");
    }

    void Container::save(ESMWriter &esm) const
    {
        esm.writeHNCString("MODL", mModel);
        esm.writeHNOCString("FNAM", mName);
        esm.writeHNT("CNDT", mWeight, 4);
        esm.writeHNT("FLAG", mFlags, 4);

        esm.writeHNOCString("SCRI", mScript);

        mInventory.save(esm);
    }

    void Container::blank()
    {
        mName.clear();
        mModel.clear();
        mScript.clear();
        mWeight = 0;
        mFlags = 0;
        mInventory.mList.clear();
    }
}
