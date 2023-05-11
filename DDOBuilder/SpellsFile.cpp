// SpellsFile.cpp
//
#include "StdAfx.h"
#include "SpellsFile.h"
#include "XmlLib\SaxReader.h"
#include "LogPane.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"Spells"; // root element name to look for
}

SpellsFile::SpellsFile(const std::string& filename, bool bItemClickies) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0),
    m_bItemClickies(bItemClickies)
{
}

SpellsFile::~SpellsFile(void)
{
}

void SpellsFile::Read()
{
    // set up a reader with this as the expected root node
    XmlLib::SaxReader reader(this, f_saxElementName);
    // read in the xml from a file (fully qualified path)
    bool ok = reader.Open(m_filename);
    if (!ok)
    {
        std::string errorMessage = reader.ErrorMessage();
        // document has failed to load. Tell the user what we can about it
        CString text;
        text.Format("The document %s\n"
                "failed to load. The XML parser reported the following problem:\n"
                "\n", m_filename.c_str());
        text += errorMessage.c_str();
        AfxMessageBox(text, MB_ICONERROR);
    }
}

XmlLib::SaxContentElementInterface * SpellsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        Spell spell;
        if (spell.SaxElementIsSelf(name, attributes))
        {
            m_loadedSpells.push_back(spell);
            subHandler = &(m_loadedSpells.back());
            // update log during load action
            CString strSpellCount;
            if (m_bItemClickies)
            {
                strSpellCount.Format("Loading Item Clickies...%d", m_loadedSpells.size());
            }
            else
            {
                strSpellCount.Format("Loading Spells...%d", m_loadedSpells.size());
            }
            GetLog().UpdateLastLogEntry(strSpellCount);
        }
    }

    return subHandler;
}

void SpellsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<Spell> & SpellsFile::Spells() const
{
    return m_loadedSpells;
}
