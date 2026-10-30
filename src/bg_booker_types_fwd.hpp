#pragma once

// Includes and forward declarations for the Booker_DB data classes, so the area headers
// (bg_booker_types_*.hpp) can point at each other's classes. Include bg_booker_data_base.hpp, not these.

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include "bg_attributes_logic.hpp"

using namespace godot;

class BG_PortraitDetails;
class BG_CharacterDetails;
class BG_StoryboardCharacterTextLocationDetails;
class BG_StoryboardDataDetails;
class BG_StoryboardDetails;
class BG_TutorialCardDetails;
class BG_TutorialCardsDetails;
class BG_IconDetails;
class BG_TwoDer_DataEntry;
class BG_LocalizeEntryData;
class BG_BaseStat;
class BG_ContentStat;
class BG_PuzzleDetails_MiscParams;
class BG_PuzzleDetails;
class BG_BattleBoard_HexTypeVisualDetails;
class BG_BattleBoard_HexTypeDetails;
class BG_BattleBoardDetails;
class BG_GameMapNodeDetails;
class BG_GameMapDetails;
class BG_RewardItem;
class BG_ObjectiveTimeline;
class BG_ObjectiveDetails;
class BG_ResourceTypeDetails;
class BG_MailData;
class BG_MarketplaceEntryData;
class BG_MarketplaceData;
class BG_MarketplaceSaveData;
class BG_AudioDataDetails;
class BG_AudioData;
class BG_HueShiftData;
class BG_EffectRarityDetails;
class BG_Effect;
class BG_DamageContainer;
class BG_RarityDetails;
class BG_LoreRarity;
class BG_UnitStatDetails;
class BG_UnitStat;
class BG_AnimationTagDetails;
class BG_AnimationDetails;
class BG_ItemSlotType;
class BG_Item;
class BG_ItemDetails;
class BG_ItemDropPool;
class BG_JobDistributionForAct;
class BG_JobMonsterDetails;
class BG_EventResourceDetails;
class BG_JobDetails;
class BG_UnitCaste;
class BG_BandMember;
class BG_Formation;
class BG_DefaultFormations;
class BG_Band;
class BG_BandNameInfo;
class BG_Job;
class BG_Monster;
class BG_BandInfo;
class BG_ActStats;
class BG_BookerSkillTreeSlotDetails;
class BG_CityInfo;
