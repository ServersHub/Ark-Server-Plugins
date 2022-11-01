#include "Player.h"
#include "Database.h"

namespace LootBox
{

	void Player::Rename(std::string newName)
	{
		m_characterName = newName;
		m_pDB->UpdateCharacterName(m_steamId, std::move(newName));
	}

	bool Player::HasBoxes(const std::string& boxName, int amount) const
	{
		auto it = m_lootboxes.find(boxName);
		if (it != m_lootboxes.end())
		{
			return it->get<int>() >= amount;
		}
		else
		{
			return false;
		}
	}

	void Player::AddBoxes(std::string boxName, int amount)
	{
		auto it = m_lootboxes.find(boxName);
		if (it != m_lootboxes.end())
		{
			*it = it->get<int>() + amount;
		}
		else
		{
			m_lootboxes[boxName] = amount;
		}
		m_pDB->UpdateLootBoxes(m_steamId, m_lootboxes);
	}

	void Player::SetBoxes(const std::string& boxName, int amount)
	{
		m_lootboxes[boxName] = amount;
		m_pDB->UpdateLootBoxes(m_steamId, m_lootboxes);
	}

	void Player::RemoveBoxes(const std::string& boxName, int amount)
	{
		auto it = m_lootboxes.find(boxName);
		if (it != m_lootboxes.end())
		{
			if (it->get<int>() > amount)
			{
				*it = it->get<int>() - amount;
			}
			else
			{
				m_lootboxes.erase(it);
			}
			m_pDB->UpdateLootBoxes(m_steamId, m_lootboxes);
		}
	}

	bool Player::HasCooldown(const std::string& boxName) const
	{
		return m_cooldowns.find(boxName) != m_cooldowns.end();
	}

	void Player::SetCooldown(const std::string& boxName, int hitsAmount)
	{
		m_cooldowns[boxName] = hitsAmount;
		m_pDB->UpdateCooldowns(m_steamId, m_cooldowns);
	}

	void Player::DecrementCooldowns()
	{
		for (auto it = m_cooldowns.begin(); it != m_cooldowns.end();)
		{
			*it = it->get<int>() - 1;
			if (it->get<int>() <= 0)
			{
				it = m_cooldowns.erase(it);
			}
			else
			{
				it++;
			}
		}
		m_pDB->UpdateCooldowns(m_steamId, m_cooldowns);
	}

}