#include "NotificationsManager.h"

namespace Protection
{
	static const int CHECK_INTERVAL = 600;

	NotificationsManager::PlayerTimings::PlayerTimings(NotificationsManager* manager, PlayerItemType player)
		: manager_(manager), player_(player)
	{
	}

	PlayerItemType NotificationsManager::PlayerTimings::GetPlayer()
	{
		return player_;
	}

	bool NotificationsManager::PlayerTimings::IsAllowed(ProtectionType protection_type, EntityType entity_type, void* ptr)
	{
		TimingsList& list = GetTimingsList(protection_type, entity_type);
		if(Timing* timing = FindTiming(list, ptr))
		{
			const int interval = GetInterval(protection_type, entity_type);
			return time(nullptr) - timing->second > interval;
		}
		else
		{
			return true;
		}
	}

	void NotificationsManager::PlayerTimings::OnNotified(ProtectionType protection_type, EntityType entity_type, void* ptr)
	{
		TimingsList& list = GetTimingsList(protection_type, entity_type);
		if(Timing* timing = FindTiming(list, ptr))
		{
			timing->second = time(nullptr);
		}
		else
		{
			list.push_back({ptr, time(nullptr)});
		}
	}

	void NotificationsManager::PlayerTimings::Update(std::time_t current_time)
	{
		UpdateTimingsList(GetTimingsList(ProtectionType::AP, EntityType::Character), current_time, GetInterval(ProtectionType::AP, EntityType::Character));
		UpdateTimingsList(GetTimingsList(ProtectionType::AP, EntityType::Dino), current_time, GetInterval(ProtectionType::AP, EntityType::Dino));
		UpdateTimingsList(GetTimingsList(ProtectionType::AP, EntityType::Structure), current_time, GetInterval(ProtectionType::AP, EntityType::Structure));
		UpdateTimingsList(GetTimingsList(ProtectionType::ORP, EntityType::Character), current_time, GetInterval(ProtectionType::ORP, EntityType::Character));
		UpdateTimingsList(GetTimingsList(ProtectionType::ORP, EntityType::Dino), current_time, GetInterval(ProtectionType::ORP, EntityType::Dino));
		UpdateTimingsList(GetTimingsList(ProtectionType::ORP, EntityType::Structure), current_time, GetInterval(ProtectionType::ORP, EntityType::Structure));
	}

	NotificationsManager::PlayerTimings::TimingsList& NotificationsManager::PlayerTimings::GetTimingsList(ProtectionType protection_type, EntityType entity_type)
	{
		return matrix_[static_cast<std::size_t>(protection_type)][static_cast<std::size_t>(entity_type)];
	}

	NotificationsManager::PlayerTimings::Timing* NotificationsManager::PlayerTimings::FindTiming(TimingsList& list, void* ptr)
	{
		auto it = std::find_if(list.begin(), list.end(), [ptr](const Timing& item) {
			return item.first == ptr;
		});
		if(it != list.cend())
		{
			return &(*it);
		}
		else
		{
			return nullptr;
		}
	}

	int NotificationsManager::PlayerTimings::GetInterval(ProtectionType protection_type, EntityType entity_type)
	{
		return manager_->GetInterval(protection_type, entity_type);
	}

	void NotificationsManager::PlayerTimings::UpdateTimingsList(TimingsList& list, std::time_t current_time, int interval)
	{
		for(auto it = list.begin(); it != list.end();)
		{
			if(current_time - it->second > interval)
			{
				it = list.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	NotificationsManager::NotificationsManager(APTimingRules* ap_rules, ORPTimingRules* orp_rules)
		: ap_rules_(ap_rules), orp_rules_(orp_rules), last_check_(time(nullptr))
	{
	}

	NotificationsManager::~NotificationsManager()
	{
	}

	bool NotificationsManager::IsAllowed(const PlayerItemType& player, ProtectionType protection_type, EntityType entity_type, void* ptr)
	{
		if(PlayerTimings* found = FindPlayer(player))
		{
			return found->IsAllowed(protection_type, entity_type, ptr);
		}
		else
		{
			return true;
		}
	}

	void NotificationsManager::OnNotified(PlayerItemType player, ProtectionType protection_type, EntityType entity_type, void* ptr)
	{
		if(PlayerTimings* found = FindPlayer(player))
		{
			found->OnNotified(protection_type, entity_type, ptr);
		}
		else
		{
			PlayerTimings& inserted = players_.emplace_back(this, player);
			inserted.OnNotified(protection_type, entity_type, ptr);
		}
	}

	void NotificationsManager::Update()
	{
		std::time_t now = time(nullptr);
		if(now >= last_check_ + CHECK_INTERVAL)
		{
			for(auto& player : players_)
			{
				player.Update(now);
			}
			last_check_ = now;
		}
	}

	int NotificationsManager::GetInterval(ProtectionType protection_type, EntityType entity_type) const
	{
		if(protection_type == ProtectionType::AP)
		{
			return GetAPInterval(entity_type);
		}
		else
		{
			return GetORPInterval(entity_type);
		}
	}

	int NotificationsManager::GetAPInterval(EntityType entity_type) const
	{
		switch (entity_type)
		{
		case EntityType::Character:
			return ap_rules_->GetPlayersNotificationsIntervalSec();
		case EntityType::Dino:
			return ap_rules_->GetDinosNotificationsIntervalSec();
		case EntityType::Structure:
			return ap_rules_->GetStructuresNotificationsIntervalSec();
		default:
			return 0;
		}
	}

	int NotificationsManager::GetORPInterval(EntityType entity_type) const
	{
		switch (entity_type)
		{
		case EntityType::Character:
			return orp_rules_->GetPlayersNotificationsIntervalSec();
		case EntityType::Dino:
			return orp_rules_->GetDinosNotificationsIntervalSec();
		case EntityType::Structure:
			return orp_rules_->GetStructuresNotificationsIntervalSec();
		default:
			return 0;
		}
	}

	NotificationsManager::PlayerTimings* NotificationsManager::FindPlayer(const PlayerItemType& player)
	{
		auto it = std::find_if(players_.begin(), players_.end(), [&player](PlayerTimings& item) {
			return player->GetSteamId() == item.GetPlayer()->GetSteamId();
		});

		if(it != players_.end())
		{
			return &(*it);
		}

		return nullptr;
	}

}