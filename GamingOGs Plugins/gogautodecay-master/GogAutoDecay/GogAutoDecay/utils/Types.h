#pragma once

#include <memory>

namespace AutoDecay
{
	template <class T, class Deleter = std::default_delete<T>>
	using ScopePtr = std::unique_ptr<T, Deleter>;

	template <class T>
	using RefPtr = std::shared_ptr<T>;
}