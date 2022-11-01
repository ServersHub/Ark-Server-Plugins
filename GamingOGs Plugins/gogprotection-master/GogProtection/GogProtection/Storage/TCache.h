#ifndef _GOG_PROT_TCACHE_H_
#define _GOG_PROT_TCACHE_H_

#include <vector>
#include <memory>
#include "DataTypes.h"
#include "Database.h"

#pragma warning(disable : 4834)

namespace Protection
{
	template <class _DataType, class _KeyType, class _DBAdapterType>
	class TCache
	{
	private:
		std::vector<std::shared_ptr<_DataType>> data_;

		struct SearchPredicate
		{
			bool operator()(const std::shared_ptr<_DataType>& lhs, _KeyType rhs) const
			{
				return lhs->GetKeyField() < rhs;
			}

			bool operator()(_KeyType lhs, const std::shared_ptr<_DataType>& rhs) const
			{
				return lhs < rhs->GetKeyField();
			}
		};

	public:
		TCache() = default;
		~TCache() = default;

		using ItemType = std::shared_ptr<_DataType>;
		using Iterator = typename decltype(data_)::iterator;
		using ConstIterator = typename decltype(data_)::const_iterator;

		void LoadData(DB::Database* db)
		{
			_DBAdapterType db_adapter(&data_, db);
			db_adapter.Load();
		}

		void SaveData(DB::Database* db)
		{
			_DBAdapterType db_adapter(&data_, db);
			db_adapter.Clean();
			db_adapter.Save();
		}

		ItemType Get(_KeyType key)
		{
			std::pair<Iterator, Iterator> res = std::equal_range(Begin(), End(), key, SearchPredicate{});

			if (std::distance(res.first, res.second) > 0)
				return *res.first;
			else
				return nullptr;
		}

		Iterator Begin()
		{
			return data_.begin();
		}

		Iterator End()
		{
			return data_.end();
		}

		ConstIterator CBegin()
		{
			return data_.cbegin();
		}

		ConstIterator CEnd()
		{
			return data_.cend();
		}

		void Insert(ItemType item)
		{
			if(Get(item->GetKeyField()))
				return;

			Iterator pos = std::lower_bound(Begin(), End(), item->GetKeyField(), SearchPredicate{});
			data_.insert(pos, item);
		}

		void Remove(_KeyType key)
		{
			std::pair<Iterator, Iterator> res = std::equal_range(Begin(), End(), key, SearchPredicate{});
			data_.erase(res.first, res.second);
		}
	};
}

#endif // !_GOG_PROT_TCACHE_H_
