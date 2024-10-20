// Life.cpp
//
#include "StdAfx.h"
#include "Life.h"
#include "XmlLib\SaxWriter.h"
#include "DDOBuilderDoc.h"
#include "Feat.h"
#include "GlobalSupportFunctions.h"
#include "LogPane.h"

#define DL_ELEMENT Life

namespace
{
    const wchar_t f_saxElementName[] = L"Life";
    DL_DEFINE_NAMES(Life_PROPERTIES)
    const unsigned f_verCurrent = 1;
}

Life::Life(Character * pCharacter) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_SpecialFeats(L"SpecialFeats"),
    m_pCharacter(pCharacter),
    m_bonusRacialActionPoints(0),
    m_bonusUniversalActionPoints(0),
    m_bonusDestinyActionPoints(0)
{
    DL_INIT(Life_PROPERTIES)
    m_Race = "Human";
}

DL_DEFINE_ACCESS(Life_PROPERTIES)

XmlLib::SaxContentElementInterface * Life::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Life_PROPERTIES)

    return subHandler;
}

void Life::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Life_PROPERTIES)
}

void Life::LoadComplete()
{
    // handle file upgrades here after load completes

    // after load if we have any level up options, this is an older file
    // format and these items need to move down into the Build objects
    if (HasLevel4())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(4, Level4(), true);
        }
        Clear_Level4();
    }
    if (HasLevel8())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(8, Level8(), true);
        }
        Clear_Level8();
    }
    if (HasLevel12())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(12, Level12(), true);
        }
        Clear_Level12();
    }
    if (HasLevel16())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(16, Level16(), true);
        }
        Clear_Level16();
    }
    if (HasLevel20())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(20, Level20(), true);
        }
        Clear_Level20();
    }
    if (HasLevel24())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(24, Level24(), true);
        }
        Clear_Level24();
    }
    if (HasLevel28())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(28, Level28(), true);
        }
        Clear_Level28();
    }
    if (HasLevel32())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(32, Level32(), true);
        }
        Clear_Level32();
    }
    if (HasLevel36())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(36, Level36(), true);
        }
        Clear_Level36();
    }
    if (HasLevel40())
    {
        for (auto& bit : m_Builds)
        {
            bit.SetAbilityLevelUp(40, Level40(), true);
        }
        Clear_Level40();
    }
    for (auto& bit: m_Builds)
    {
        bit.LoadComplete();
    }
}

void Life::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName(), VersionAttributes());
    DL_WRITE(Life_PROPERTIES)
    writer->EndElement();
}

CString Life::UIDescription(size_t index) const
{
    CString strDescription;
    if (m_Name != "")
    {
        strDescription = m_Name.c_str();
    }
    else
    {
        strDescription.Format("Life %d", index + 1);
    }
    return strDescription;
}

void Life::SetName(const std::string& name)
{
    m_Name = name;
}

const Build & Life::GetBuild(size_t buildIndex) const
{
    static Build badBuild(const_cast<Life*>(this));
    if (buildIndex < m_Builds.size())
    {
        std::list<Build>::const_iterator bit = m_Builds.begin();
        std::advance(bit, buildIndex);
        return (*bit);
    }
    ASSERT(FALSE);
    return badBuild;
}

Build * Life::GetBuildPointer(size_t buildIndex)
{
    Build * pBuild = NULL;
    if (buildIndex < m_Builds.size())
    {
        std::list<Build>::iterator bit = m_Builds.begin();
        std::advance(bit, buildIndex);
        pBuild = &(*bit);
    }
    return pBuild;
}

const Build * Life::GetBuildPointer(size_t buildIndex) const
{
    const Build * pBuild = NULL;
    if (buildIndex < m_Builds.size())
    {
        std::list<Build>::const_iterator bit = m_Builds.begin();
        std::advance(bit, buildIndex);
        pBuild = &(*bit);
    }
    return pBuild;
}

size_t Life::AddBuild(size_t buildIndex)
{
    size_t newIndex = 0;
    // copy the build if there is one, else add a blank one
    if (m_Builds.size() > buildIndex)
    {
        std::list<Build>::const_iterator bit = m_Builds.begin();
        std::advance(bit, buildIndex);
        Build newBuild = *bit;
        bit++;
        // insert it immediately after the current build
        m_Builds.insert(bit, newBuild);
        newIndex = buildIndex+1;
    }
    else
    {
        m_Builds.push_back(Build(this));
    }
    return newIndex;
}

void Life::DeleteBuild(size_t buildIndex)
{
    std::list<Build>::iterator bit = m_Builds.begin();
    std::advance(bit, buildIndex);
    m_Builds.erase(bit);
}

void Life::SetBuildLevel(size_t buildIndex, size_t level)
{
    std::list<Build>::iterator bit = m_Builds.begin();
    std::advance(bit, buildIndex);
    (*bit).SetLevel(level);
    Build* pBuild = &*bit;  // we need to check the index
    // all builds are sort by level
    m_Builds.sort();
    size_t index = 0;
    for (auto&& bit2 : m_Builds)
    {
        if (pBuild == &bit2)
        {
            break;
        }
        ++index;
    }
    m_pCharacter->SetActiveBuildIndex(index);
}

int Life::TomeAtLevel(
        AbilityType ability,
        size_t level) const
{
    // Update 37 Patch 1 changed the way tomes apply
    // they now apply at:
    // +1-+2 Level 1
    // +3 Level 3
    // +4 Level 7
    // +5 Level 11
    // +6 Level 15
    // +7 Level 19
    // +8 or higher Level 22
    // level is 1 based for this calculation
    ++level;
    size_t maxTome = AbilityTomeValue(ability);
    size_t maxAtLevel = 0;
    switch (level)
    {
    case 1:
    case 2:
        maxAtLevel = 2;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        maxAtLevel = 3;
        break;
    case 7:
    case 8:
    case 9:
    case 10:
        maxAtLevel = 4;
        break;
    case 11:
    case 12:
    case 13:
    case 14:
        maxAtLevel = 5;
        break;
    case 15:
    case 16:
    case 17:
    case 18:
        maxAtLevel = 6;
        break;
    case 19:
    case 20:
    case 21:
        maxAtLevel = 7;
        break;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    default:
        maxAtLevel = 999;   // no upper limit
        break;

    }
    size_t tv = min(maxTome, maxAtLevel);
    return tv;
}

void Life::SetAbilityTome(AbilityType ability, size_t value)
{
    if (value != AbilityTomeValue(ability))
    {
        switch (ability)
        {
        case Ability_Strength:
            Set_StrTome(value);
            break;
        case Ability_Dexterity:
            Set_DexTome(value);
            break;
        case Ability_Constitution:
            Set_ConTome(value);
            break;
        case Ability_Intelligence:
            Set_IntTome(value);
            break;
        case Ability_Wisdom:
            Set_WisTome(value);
            break;
        case Ability_Charisma:
            Set_ChaTome(value);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
        // if Intelligence has changed, update the available skill points
        if (ability == Ability_Intelligence)
        {
            UpdateSkillPoints();
        }
        NotifyAbilityTomeChanged(ability);
        SetModifiedFlag(TRUE);
        // changing an inherent tome value can invalidate a feat selection (e.g Dodge)
        VerifyTrainedFeats();
    }
}

size_t Life::AbilityTomeValue(AbilityType ability) const
{
    size_t value = 0;
    switch (ability)
    {
    case Ability_Strength:
        value = StrTome();
        break;
    case Ability_Dexterity:
        value = DexTome();
        break;
    case Ability_Constitution:
        value = ConTome();
        break;
    case Ability_Intelligence:
        value = IntTome();
        break;
    case Ability_Wisdom:
        value = WisTome();
        break;
    case Ability_Charisma:
        value = ChaTome();
        break;
    }
    return value;
}

void Life::UpdateSkillPoints()
{
    // update all the build objects we have
    std::list<Build>::iterator bit = m_Builds.begin();
    while (bit != m_Builds.end())
    {
        (*bit).UpdateSkillPoints();
        ++bit;
    }
}

void Life::VerifyTrainedFeats()
{
    // update all the build objects we have
    std::list<Build>::iterator bit = m_Builds.begin();
    while (bit != m_Builds.end())
    {
        (*bit).VerifyTrainedFeats();
        ++bit;
    }
}

void Life::NotifyAbilityTomeChanged(AbilityType ability)
{
    NotifyAll(&LifeObserver::UpdateAbilityTomeChanged, this, ability);
}

void Life::NotifyLifeFeatTrained(const std::string& featName)
{
    NotifyAll(&LifeObserver::UpdateLifeFeatTrained, this, featName);
}

void Life::NotifyLifeFeatRevoked(const std::string& featName)
{
    NotifyAll(&LifeObserver::UpdateLifeFeatRevoked, this, featName);
}

void Life::NotifyAlignmentChanged(AlignmentType alignment)
{
    NotifyAll(&LifeObserver::UpdateAlignmentChanged, this, alignment);
}

void Life::NotifyRaceChanged(const std::string& race)
{
    NotifyAll(&LifeObserver::UpdateRaceChanged, this, race);
}

void Life::NotifyActionPointsChanged()
{
    NotifyAll(&LifeObserver::UpdateActionPointsChanged, this);
}

void Life::NotifySkillTomeChanged(SkillType skill)
{
    NotifyAll(&LifeObserver::UpdateSkillTomeChanged, this, skill);
}

void Life::SetModifiedFlag(BOOL modified)
{
    m_pCharacter->SetModifiedFlag(modified);
}

int Life::BonusRacialActionPoints() const
{
    return m_bonusRacialActionPoints;
}

int Life::BonusUniversalActionPoints() const
{
    return m_bonusUniversalActionPoints;
}

int Life::BonusDestinyActionPoints() const
{
    return m_bonusDestinyActionPoints;
}

void Life::SetSkillTome(SkillType skill, size_t value)
{
    m_Tomes.SetSkillTome(skill, value);
    NotifySkillTomeChanged(skill);
}

size_t Life::SkillTomeValue(SkillType skill, size_t level) const
{
    // max tome value for level applies
    // +1 and +2 Tomes will be applied at level 1 and higher
    // +3 Tomes will be applied at level 3 and higher
    // +4 Tomes will be applied at level 7 and higher
    // +5 Tomes will be applied at level 11 and higher
    size_t maxTome = 2;
    if (level >= 3) ++maxTome;
    if (level >= 7) ++maxTome;
    if (level >= 11) ++maxTome;
    if (level >= 15) ++maxTome;     // assumed progression
    if (level >= 19) ++maxTome;
    if (level >= 23) ++maxTome;
    if (level >= 27) ++maxTome;
    if (level >= 31) ++maxTome;
    size_t value = m_Tomes.SkillTomeValue(skill);
    value = min(maxTome, value);
    return value;
}

void Life::SetRace(const std::string& race)
{
    Set_Race(race);
    for (auto&& bit : m_Builds)
    {
        // all builds need to update their skill points
        bit.UpdateSkillPoints();
        bit.UpdateFeats(true);
        bit.VerifyGear();
    }
    NotifyRaceChanged(race);
}

void Life::SetAlignment(AlignmentType alignment)
{
    Set_Alignment(alignment);
    NotifyAlignmentChanged(alignment);
}

size_t Life::GetSpecialFeatTrainedCount(
        const std::string& featName) const
{
    // return the count of how many times this particular feat has
    // been trained.
    size_t count = 0;
    const std::list<TrainedFeat>& specialFeats = SpecialFeats().Feats();
    for (auto&& sfit: specialFeats)
    {
        if (sfit.FeatName() == featName)
        {
            ++count;    // it is present, count it
        }
    }
    // also need to check the builds favor feats
    const Build* pBuild = m_pCharacter->ActiveBuild();
    if (pBuild != NULL)
    {
        const std::list<TrainedFeat>& favorFeats = pBuild->FavorFeats().Feats();
        for (auto&& ffit : favorFeats)
        {
            if (ffit.FeatName() == featName)
            {
                ++count;    // it is present, count it
            }
        }
    }
    return count;
}

void Life::TrainSpecialFeat(
        const std::string& featName)
{
    const Feat& feat = FindFeat(featName);
    // just add a copy of the feat name to the current list
    TrainedFeat tf(featName, (LPCTSTR)EnumEntryText(feat.Acquire(), featAcquisitionMap), 0);
    m_SpecialFeats.Add(tf);
    m_hasSpecialFeats = true;

    // notify about the feat effects
    ApplyFeatEffects(feat);

    NotifyLifeFeatTrained(featName);
    m_pCharacter->SetModifiedFlag(TRUE);

    // add log entry
    std::stringstream ss;
    ss << "Trained the special feat \"" << featName.c_str() << "\"";
    GetLog().AddLogEntry(ss.str().c_str());

    // number of past lives affects how many build points they have to spend
    //DetermineBuildPoints();
    //DetermineFatePoints();
    //DetermineEpicCompletionist();

    // special feats may change the number of action points available
    CountBonusRacialAP();
    CountBonusUniversalAP();
}

void Life::RevokeSpecialFeat(
        const std::string& featName)
{
    // just remove the first copy of the feat name from the current list
    std::list<TrainedFeat> trainedFeats = SpecialFeats().Feats();
    std::list<TrainedFeat>::iterator it = trainedFeats.begin();
    bool found = false;
    while (!found && it != trainedFeats.end())
    {
        if ((*it).FeatName() == featName)
        {
            // this is the first occurrence, remove it
            it = trainedFeats.erase(it);
            found = true;
        }
        else
        {
            ++it;
        }
    }
    if (found)
    {
        FeatsListObject flo(L"SpecialFeats", trainedFeats);
        Set_SpecialFeats(flo);
        // notify about the feat effects
        const Feat & feat = FindFeat(featName);
        RevokeFeatEffects(feat);
        NotifyLifeFeatRevoked(featName);
        // add log entry
        std::stringstream ss;
        ss << "Revoked the special feat \"" << featName.c_str() << "\"";
        GetLog().AddLogEntry(ss.str().c_str());
        //m_pDocument->SetModifiedFlag(TRUE);
        //// number of past lives affects how many build points they have to spend
        //DetermineBuildPoints();
        //DetermineFatePoints();
        //DetermineEpicCompletionist();
        //// special feats may change the number of action points available
        CountBonusRacialAP();
        CountBonusUniversalAP();
        //if (feat.Acquire() == FeatAcquisition_RacialPastLife)
        //{
        //    std::list<TrainedFeat> allFeats = SpecialFeats().Feats();
        //    UpdateFeats(0, &allFeats);      // racial completionist state may have changed
        //}
        //if (feat.Acquire() == FeatAcquisition_HeroicPastLife)
        //{
        //    // revoking a special feat in theory can invalidate a feat selection (e.g. completionist)
        //    VerifyTrainedFeats();
        //}
    }
}

void Life::ApplyFeatEffects(const Feat& feat)
{
    // this feats effects need to be applied to all builds
    for (auto&& bit: m_Builds)
    {
        Build *pBuild = &bit;
        pBuild->ApplyFeatEffects(feat);
    }
}

void Life::RevokeFeatEffects(const Feat& feat)
{
    // this feats effects need to be revoked from all builds
    for (auto&& bit: m_Builds)
    {
        bit.RevokeFeatEffects(feat);
    }
}

void Life::CountBonusRacialAP()
{
    // look through the list of all special feats and count how many bonus AP there are
    // if the number has changed from what we currently have, change it and notify
    int APcount = 0;
    const std::list<Feat>& racialFeats = RacialPastLifeFeats();
    for (auto&& fi : racialFeats)
    {
        size_t count = GetSpecialFeatTrainedCount(fi.Name());
        if (count > 0)
        {
            // look at all the feat effects and see if any affect our AP count
            const std::list<Effect>& effects = fi.Effects();
            for (auto&& ei: effects)
            {
                if (!ei.HasRank() || ei.Rank() == count)
                {
                    if (ei.IsType(Effect_RAPBonus))
                    {
                        // APs are always whole numbers
                        APcount += (size_t)ei.Amount()[count-1];
                    }
                }
            }
        }
    }
    const std::list<Feat>& specialFeats = ::SpecialFeats();
    for (auto&& fi : specialFeats)
    {
        size_t count = GetSpecialFeatTrainedCount(fi.Name());
        if (count > 0)
        {
            // look at all the feat effects and see if any affect our AP count
            const std::list<Effect>& effects = fi.Effects();
            for (auto&& ei: effects)
            {
                if (!ei.HasRank() || ei.Rank() == count)
                {
                    if (ei.IsType(Effect_RAPBonus))
                    {
                        // APs are always whole numbers
                        APcount += (size_t)ei.Amount()[count-1];
                    }
                }
            }
        }
    }
    if (APcount != m_bonusRacialActionPoints)
    {
        m_bonusRacialActionPoints = APcount;
        NotifyActionPointsChanged();
    }
}

void Life::CountBonusUniversalAP()
{
    // look through the list of all special feats and count how many bonus AP there are
    // if the number has changed from what we currently have, change it and notify
    int UAPcount = 0;
    int DAPcount = 0;
    const std::list<Feat>& specialFeats = ::SpecialFeats();
    for (auto&& fi: specialFeats)
    {
        size_t count = GetSpecialFeatTrainedCount(fi.Name());
        if (count > 0)
        {
            // look at all the feat effects and see if any affect our AP count
            const std::list<Effect>& effects = fi.Effects();
            for (auto&& ei: effects)
            {
                if (!ei.HasRank() || ei.Rank() == count)
                {
                    if (ei.IsType(Effect_UAPBonus))
                    {
                        // APs are always whole numbers
                        UAPcount += (size_t)ei.Amount()[count-1];
                    }
                    if (ei.IsType(Effect_DestinyAPBonus))
                    {
                        // APs are always whole numbers
                        DAPcount += (size_t)ei.Amount()[count-1];
                    }
                }
            }
        }
    }
    if (UAPcount != m_bonusUniversalActionPoints
            || DAPcount != m_bonusDestinyActionPoints)
    {
        m_bonusUniversalActionPoints = UAPcount;
        m_bonusDestinyActionPoints = DAPcount;
        NotifyActionPointsChanged();
    }
}

void Life::NotifyActiveBuildChanged()
{
    m_bonusUniversalActionPoints = 0;
    m_bonusDestinyActionPoints = 0;
    m_pCharacter->NotifyActiveBuildChanged();
    CountBonusRacialAP();
    CountBonusUniversalAP();
}

bool Life::ApplyGuildBuffs() const
{
    bool bApply = false;
    if (m_pCharacter->HasApplyGuildBuffs())
    {
        bApply = m_pCharacter->ApplyGuildBuffs();
    }
    return bApply;
}

size_t Life::GuildLevel() const
{
    size_t guildLevel = 0;
    if (m_pCharacter->HasApplyGuildBuffs())
    {
        if (m_pCharacter->HasGuildLevel())
        {
            guildLevel = m_pCharacter->GuildLevel();
        }
    }
    return guildLevel;
}

void Life::EnableSelfAndPartyBuff(const std::string& name)
{
    // add it to the list of enabled buffs
    m_SelfAndPartyBuffs.push_front(name);
    Build* pBuild = m_pCharacter->ActiveBuild();
    pBuild->NotifyOptionalBuff(name);
    m_pCharacter->SetModifiedFlag(TRUE);
}

void Life::DisableSelfAndPartyBuff(const std::string& name)
{
    // remove it from the list of enabled buffs
    bool found = false;
    std::list<std::string>::iterator it = m_SelfAndPartyBuffs.begin();
    while (!found && it != m_SelfAndPartyBuffs.end())
    {
        if ((*it) == name)
        {
            // this is the one to remove
            found = true;
            it = m_SelfAndPartyBuffs.erase(it);
        }
        else
        {
            ++it;
        }
    }
    Build* pBuild = m_pCharacter->ActiveBuild();
    pBuild->RevokeOptionalBuff(name);
    m_pCharacter->SetModifiedFlag(TRUE);
}

void Life::UpdateBuildLifePointers(Character* pCharacter)
{
    m_pCharacter = pCharacter;
    for (auto&& it: m_Builds)
    {
        it.SetLifePointer(this);
    }
}

