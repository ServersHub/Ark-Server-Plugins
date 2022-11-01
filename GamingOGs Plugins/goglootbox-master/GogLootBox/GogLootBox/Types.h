#ifndef _LOOTBOX_TYPES_H_
#define _LOOTBOX_TYPES_H_

#include <json.hpp>
#include <optional>
#include <memory>
#include <vector>

namespace LootBox
{
	class Player;
	class Box;
	class BoxElementBase;
	class Item;
	class Dino;

	using PlayersList = std::vector<std::shared_ptr<Player>>;
	using BoxesList = std::vector<std::shared_ptr<Box>>;
	using ElementsList = std::vector<std::shared_ptr<BoxElementBase>>;
	using ItemsList = std::vector<std::shared_ptr<Item>>;
	using DinosList = std::vector<std::shared_ptr<Dino>>;

	using Json = nlohmann::basic_json<>;
	using JsonPointer = nlohmann::json_pointer<nlohmann::basic_json<>>;

	template <class T>
	using Optional = std::optional<T>;
}

#endif // !_LOOTBOX_TYPES_H_

