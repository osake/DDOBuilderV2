// Character.cpp
//
#include "StdAfx.h"
#include "Character.h"
#include "XmlLib\SaxWriter.h"
#include "DDOBuilderDoc.h"

#define DL_ELEMENT Character

namespace
{
    const wchar_t f_saxElementName[] = L"Character";
    DL_DEFINE_NAMES(Character_PROPERTIES)
    const unsigned f_verCurrent = 1;
}

Character::Character(CDDOBuilderDoc * pDoc) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_pDocument(pDoc),
    m_activeLifeIndex(0),
    m_activeBuildIndex(0)
{
    DL_INIT(Character_PROPERTIES)
}

DL_DEFINE_ACCESS(Character_PROPERTIES)

XmlLib::SaxContentElementInterface * Character::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Character_PROPERTIES)

    return subHandler;
}

void Character::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Character_PROPERTIES)
}

void Character::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName(), VersionAttributes());
    DL_WRITE(Character_PROPERTIES)
    writer->EndElement();
}

void Character::AboutToLoad()
{
    // if we are about to load, we reset all our contained data
    DL_INIT(Character_PROPERTIES)
    m_Lives.clear();
    // 1st build in 1st life is the default (may still not exist)
    m_activeLifeIndex = 0;
    m_activeBuildIndex = 0;
}

void Character::NotifyNumBuildsChanged()
{
    NotifyAll(&CharacterObserver::UpdateNumBuildsChanged, this);
}

void Character::NotifyActiveLifeChanged()
{
    NotifyAll(&CharacterObserver::UpdateActiveLifeChanged, this);
}

void Character::NotifyActiveBuildChanged()
{
    NotifyAll(&CharacterObserver::UpdateActiveBuildChanged, this);
}

const Life & Character::GetLife(size_t lifeIndex) const
{
    std::list<Life>::const_iterator lit = m_Lives.begin();
    std::advance(lit, lifeIndex);
    return (*lit);
}

void Character::SetLifeName(
        size_t lifeIndex,
        CString name)
{
    std::list<Life>::iterator lit = m_Lives.begin();
    std::advance(lit, lifeIndex);
    (*lit).SetName((LPCSTR)name);
    m_pDocument->SetModifiedFlag(TRUE);
}

size_t Character::AddLife()
{
    // all new lives start with a default build
    Life life(this);
    m_Lives.push_back(life);
    m_Lives.back().AddBuild();
    m_pDocument->SetModifiedFlag(TRUE);
    return m_Lives.size()-1;    // 0 based
}

void Character::DeleteLife(size_t lifeIndex)
{
    std::list<Life>::iterator lit = m_Lives.begin();
    std::advance(lit, lifeIndex);
    m_Lives.erase(lit);
    m_pDocument->SetModifiedFlag(TRUE);
}

size_t Character::AddBuild(size_t lifeIndex)
{
    std::list<Life>::iterator lit = m_Lives.begin();
    std::advance(lit, lifeIndex);
    m_pDocument->SetModifiedFlag(TRUE);
    return (*lit).AddBuild();
}

void Character::DeleteBuild(
        size_t lifeIndex,
        size_t buildIndex)
{
    std::list<Life>::iterator lit = m_Lives.begin();
    std::advance(lit, lifeIndex);
    (*lit).DeleteBuild(buildIndex);
    m_pDocument->SetModifiedFlag(TRUE);
}

size_t Character::GetBuildLevel(
        size_t lifeIndex,
        size_t buildIndex) const
{
    std::list<Life>::const_iterator clit = m_Lives.begin();
    std::advance(clit, lifeIndex);
    const Build & build = (*clit).GetBuild(buildIndex);
    return build.Level();
}

void Character::SetBuildLevel(
        size_t lifeIndex,
        size_t buildIndex,
        size_t level)
{
    std::list<Life>::iterator lit = m_Lives.begin();
    std::advance(lit, lifeIndex);
    (*lit).SetBuildLevel(buildIndex, level);
}

void Character::SetActiveBuild(size_t lifeIndex, size_t buildIndex)
{
    m_activeLifeIndex = lifeIndex;
    m_activeBuildIndex = buildIndex;
    NotifyActiveLifeChanged();
    NotifyActiveBuildChanged();
    if (ActiveBuild() != NULL)
    {
        ActiveBuild()->BuildNowActive();
    }
}

Life* Character::ActiveLife()
{
    Life* pLife = NULL;
    // find the active life
    if (m_activeLifeIndex < m_Lives.size())
    {
        std::list<Life>::iterator lit = m_Lives.begin();
        std::advance(lit, m_activeLifeIndex);
        pLife = &(*lit);
    }
    return pLife;
}

Build * Character::ActiveBuild()
{
    Build * pBuild = NULL;
    // find the active build
    if (m_activeLifeIndex < m_Lives.size())
    {
        std::list<Life>::iterator lit = m_Lives.begin();
        std::advance(lit, m_activeLifeIndex);
        pBuild = (*lit).GetBuildPointer(m_activeBuildIndex);
    }
    return pBuild;
}

const Life* Character::ActiveLife() const
{
    const Life* pLife = NULL;
    // find the active life
    if (m_activeLifeIndex < m_Lives.size())
    {
        std::list<Life>::const_iterator lit = m_Lives.begin();
        std::advance(lit, m_activeLifeIndex);
        pLife = &(*lit);
    }
    return pLife;
}

const Build * Character::ActiveBuild() const
{
    const Build * pBuild = NULL;
    // find the active build
    if (m_activeLifeIndex < m_Lives.size())
    {
        std::list<Life>::const_iterator lit = m_Lives.begin();
        std::advance(lit, m_activeLifeIndex);
        pBuild = (*lit).GetBuildPointer(m_activeBuildIndex);
    }
    return pBuild;
}

void Character::SetModifiedFlag(BOOL modified)
{
    m_pDocument->SetModifiedFlag(modified);
}

