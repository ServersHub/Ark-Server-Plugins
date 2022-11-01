#ifndef _LOOTBOX_BOX_H_
#define _LOOTBOX_BOX_H_

#include "Types.h"

namespace LootBox
{

	enum class BoxElementType
	{
		Item = 0,
		Dino,
		Beacon,
		Command,
		Points
	};

	inline BoxElementType TypeStringToEnum(const std::string& str)
	{
		if (str == "item") { return BoxElementType::Item; }
		else if (str == "dino") { return BoxElementType::Dino; }
		else if (str == "beacon") { return BoxElementType::Beacon; }
		else if (str == "command") { return BoxElementType::Command; }
		else { return BoxElementType::Points; }
	}

	class Item
	{
	public:
		Item(int minQuality, int maxQuality, int amount, Optional<int> addState, int blueprintChance, std::string blueprint)
			: m_minQuality(minQuality), m_maxQuality(maxQuality), m_amount(amount), m_addStat(addState), m_blueprintChance(blueprintChance), m_blueprint(std::move(blueprint))
		{
		}

		[[nodiscard]] int MinQuality() const noexcept { return m_minQuality; }
		[[nodiscard]] int MaxQuality() const noexcept { return m_maxQuality; }
		[[nodiscard]] int Amount() const noexcept { return m_amount; }
		[[nodiscard]] Optional<int> AddStat() const noexcept { return m_addStat; }
		[[nodiscard]] int BlueprintChance() const noexcept { return m_blueprintChance; }
		[[nodiscard]] const std::string& Blueprint() const noexcept { return m_blueprint; }

	private:
		int m_minQuality;
		int m_maxQuality;
		int m_amount;
		Optional<int> m_addStat;
		int m_blueprintChance;
		std::string m_blueprint;
	};

	class Dino
	{
	public:
		Dino(int level, bool isBaby, int imprintAmount, Optional<bool> isNeutered, std::string blueprint, Optional<std::string> saddleBlueprint, Optional<int> saddleQuality)
			: m_level(level), m_isBaby(isBaby), m_imprintAmount(imprintAmount), m_isNeutered(isNeutered), m_blueprint(std::move(blueprint)), m_saddleBlueprint(std::move(saddleBlueprint)), m_saddleQuality(std::move(saddleQuality))
		{
		}

		[[nodiscard]] int Level() const noexcept { return m_level; }
		[[nodiscard]] bool IsBaby() const noexcept { return m_isBaby; }
		[[nodiscard]] int ImprintAmount() const noexcept { return m_imprintAmount; }
		[[nodiscard]] Optional<bool> IsNeutered() const noexcept { return m_isNeutered; }
		[[nodiscard]] const std::string& Blueprint() const noexcept { return m_blueprint; }
		[[nodiscard]] const Optional<std::string>& SaddleBlueprint() const noexcept { return m_saddleBlueprint; }
		[[nodiscard]] const Optional<int>& SaddleQuality() const noexcept { return m_saddleQuality; }

	private:
		int m_level;
		bool m_isBaby;
		int m_imprintAmount;
		Optional<bool> m_isNeutered;
		std::string m_blueprint;
		Optional<std::string> m_saddleBlueprint;
		Optional<int> m_saddleQuality;
	};

	class BoxElementBase
	{
	public:
		BoxElementBase(BoxElementType type, std::string description, int probability)
			: m_type(type), m_description(std::move(description)), m_probability(probability)
		{
		}

		virtual ~BoxElementBase() {}

		[[nodiscard]] BoxElementType Type() const noexcept { return m_type; }
		[[nodiscard]] const std::string& Description() const noexcept { return m_description; }
		[[nodiscard]] int Probability() const noexcept { return m_probability; }

	private:
		BoxElementType m_type;
		std::string m_description;
		int m_probability;
	};

	class BoxElementItem : public BoxElementBase
	{
	public:
		explicit BoxElementItem(std::string description, int probability, std::vector<std::shared_ptr<Item>> items)
			: BoxElementBase(BoxElementType::Item, std::move(description), probability), m_items(std::move(items))
		{
		}

		[[nodiscard]] const ItemsList& Items() const noexcept { return m_items; }

	private:
		ItemsList m_items;
	};

	class BoxElementDino : public BoxElementBase
	{
	public:
		explicit BoxElementDino(std::string description, int probability, std::vector<std::shared_ptr<Dino>> dinos)
			: BoxElementBase(BoxElementType::Dino, std::move(description), probability), m_dinos(std::move(dinos))
		{
		}

		[[nodiscard]] const DinosList& Dinos() const noexcept { return m_dinos; }

		bool CheckCompatibility(const std::string& mapName)
		{
			for (auto it = m_dinos.begin(); it != m_dinos.end();)
			{
				if (!Configuration::Get().IsDinoAllowed(mapName, (*it)->Blueprint()))
				{
					it = m_dinos.erase(it);
				}
				else
				{
					it++;
				}
			}
			return !m_dinos.empty();
		}

	private:
		DinosList m_dinos;
	};

	class BoxElementBeacon : public BoxElementBase
	{
	public:
		explicit BoxElementBeacon(std::string description, int probability, std::string className)
			: BoxElementBase(BoxElementType::Beacon, std::move(description), probability), m_className(std::move(className))
		{
		}

		[[nodiscard]] const std::string& ClassName() const noexcept { return m_className; }

	private:
		std::string m_className;
	};

	class BoxElementCommand : public BoxElementBase
	{
	public:
		explicit BoxElementCommand(std::string description, int probability, std::string command)
			: BoxElementBase(BoxElementType::Command, std::move(description), probability), m_command(std::move(command))
		{
		}

		[[nodiscard]] const std::string& Command() const noexcept { return m_command; }

	private:
		std::string m_command;
	};

	class BoxElementPoints : public BoxElementBase
	{
	public:
		BoxElementPoints(std::string description, int probability, int minAmount, int maxAmount)
			: BoxElementBase(BoxElementType::Points, std::move(description), probability), m_minAmount(minAmount), m_maxAmount(maxAmount)
		{
		}

		[[nodiscard]] int MinAmount() const noexcept { return m_minAmount; }
		[[nodiscard]] int MaxAmount() const noexcept { return m_maxAmount; }

	private:
		int m_minAmount;
		int m_maxAmount;
	};

	class Box
	{
	public:
		Box(std::string name, int gamblePrecent, int gamblePrice, std::string permission, int minAmount, int maxAmount, ElementsList elements)
			: m_name(std::move(name)), m_gamblePercent(gamblePrecent), m_gamblePrice(gamblePrice), m_permission(std::move(permission)), m_minAmount(minAmount), m_maxAmount(maxAmount), m_elements(std::move(elements))
		{
		}

		void CheckCompatibility(const std::string& mapName)
		{
			for (auto it = m_elements.begin(); it != m_elements.end();)
			{
				if ((*it)->Type() == BoxElementType::Dino)
				{
					if (!std::static_pointer_cast<BoxElementDino>(*it)->CheckCompatibility(mapName))
					{
						it = m_elements.erase(it);
					}
					else
					{
						it++;
					}
				}
				else
				{
					it++;
				}
			}
		}

		[[nodiscard]] std::string Name() const noexcept { return m_name; }
		[[nodiscard]] int GamblePercent() const noexcept { return m_gamblePercent; }
		[[nodiscard]] int GamblePrice() const noexcept { return m_gamblePrice; }
		[[nodiscard]] const std::string& Permission() const noexcept { return m_permission; }
		[[nodiscard]] int MinAmount() const noexcept { return m_minAmount; }
		[[nodiscard]] int MaxAmount() const noexcept { return m_maxAmount; }
		[[nodiscard]] const ElementsList& Elements() const noexcept { return m_elements; }

	private:
		std::string m_name;
		int m_gamblePercent;
		int m_gamblePrice;
		std::string m_permission;
		int m_minAmount;
		int m_maxAmount;
		ElementsList m_elements;
	};

}

#endif // !_LOOTBOX_BOX_H_
