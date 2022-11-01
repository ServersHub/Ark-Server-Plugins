#pragma once

#include <vector>
#include <memory>
#include "PlayerProps.h"
#include "TeamProps.h"
#include "Database.h"

#pragma warning(disable : 4834)

namespace AutoDecay
{
	template <class _DataType, class _KeyType, class _DBAdapterType>
	class TCache
	{
	private:
		std::vector<_DataType> data_;

		struct SearchPredicate
		{
			bool operator()(const _DataType& lhs, const _DataType& rhs) const
			{
				return lhs->GetKeyField() < rhs->GetKeyField();
			}

			bool operator()(const _DataType& lhs, _KeyType rhs) const
			{
				return lhs->GetKeyField() < rhs;
			}

			bool operator()(_KeyType lhs, const _DataType& rhs) const
			{
				return lhs < rhs->GetKeyField();
			}
		};

	public:
		TCache() = default;
		~TCache() = default;

		using Iterator = typename decltype(data_)::iterator;
		using ConstIterator = typename decltype(data_)::const_iterator;

		void LoadData(Database* db)
		{
			_DBAdapterType db_adapter(&data_, db);
			db_adapter.Load();
		}

		void SaveData(Database* db)
		{
			_DBAdapterType db_adapter(&data_, db);
			db_adapter.Save();
		}

		_DataType Get(_KeyType key)
		{
			const auto pos = std::lower_bound(Begin(), End(), key, SearchPredicate{});

			if (pos != End() && (*pos)->GetKeyField() == key)
			{
				return *pos;
			}

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

		void Insert(_DataType item)
		{
			if (Get(item->GetKeyField()))
			{
				return;
			}

			const auto pos = std::lower_bound(Begin(), End(), item->GetKeyField(), SearchPredicate{});
			data_.insert(pos, item);
		}

		void Remove(_KeyType key)
		{
			const auto res = std::equal_range(Begin(), End(), key, SearchPredicate{});
			data_.erase(res.first, res.second);
		}
	};
}