#ifndef SPARSE_SET
#define SPARSE_SET

#include <vector>

#include <type_traits>
#include <limits>
#include <utility>

#include "InternalAssert.h"

namespace Internal
{
	namespace Impl
	{
		template<typename T>
		concept KeyType = std::is_integral_v<T> && std::is_unsigned_v<T> 
						&& std::is_trivially_copyable_v<T> && std::is_swappable_v<T>; //erase function

		template<typename T>
		concept MoveAssignmentVal = std::is_move_assignable_v<T>;

		//We only want to move construct when we arent able to move assign
		template<typename T>
		concept MoveConstructVal = !MoveAssignmentVal<T> && std::is_move_constructible_v<T>;

		template<typename T>
		concept ValType = std::is_nothrow_destructible_v<T> 
						&& (MoveAssignmentVal<T> || MoveConstructVal<T>);
	}

	template<Impl::ValType Val, Impl::KeyType KeyType = uint32_t>
	class sparse_set final
	{
	public:
		sparse_set() = default;
		sparse_set(KeyType sparseSize) :
			m_SparseArr(sparseSize, INVALID_INDEX)
		{ }

		~sparse_set() = default;

		using Iterator = typename std::vector<Val>::iterator;
		using ConstIterator = typename std::vector<Val>::const_iterator;
		
		using ReverseIterator = typename std::vector<Val>::reverse_iterator;
		using ConstReverseIterator = typename std::vector<Val>::const_reverse_iterator;

		Iterator begin() noexcept { return m_PackedValArr.begin(); }
		Iterator end() noexcept { return m_PackedValArr.end(); }
		ConstIterator begin() const noexcept { return m_PackedValArr.begin(); }
		ConstIterator end() const noexcept { return m_PackedValArr.end(); }

		ReverseIterator rbegin() noexcept { return m_PackedValArr.rbegin(); }
		ReverseIterator rend() noexcept { return m_PackedValArr.rend(); }
		ConstReverseIterator rbegin() const noexcept { return m_PackedValArr.rbegin(); }
		ConstReverseIterator rend() const noexcept { return m_PackedValArr.rend(); }

		[[nodiscard]] size_t size() const noexcept { return m_DenseArr.size(); }
		[[nodiscard]] size_t sparse_size() const noexcept { return m_SparseArr.size(); }

		void clear() noexcept
		{
			m_DenseArr.clear();
			m_PackedValArr.clear();
			m_SparseArr.clear();
		}

		[[nodiscard]] bool contains(KeyType element) const noexcept { return element < m_SparseArr.size() && m_SparseArr[element] != INVALID_INDEX; }

		//Do not emplace the same element in the set twice, use try_emplace if this is a concern.
		template<typename... Args>
		requires std::is_constructible_v<Val, Args...>
		Val& emplace(KeyType element, Args&&... args) noexcept
		{
			ASSERT(!contains(element), "Element already in set!");

			if (element >= m_SparseArr.size())
			{
				m_SparseArr.resize(element + 1, INVALID_INDEX);
			}

			m_SparseArr[element] = static_cast<KeyType>(m_DenseArr.size());

			m_DenseArr.emplace_back(element);
			return m_PackedValArr.emplace_back(std::forward<Args>(args)...);
		}

		template<typename... Args>
		requires std::is_constructible_v<Val, Args...>
		std::pair<Iterator, bool> try_emplace(KeyType element, Args&&... args) noexcept
		{
			if (contains(element))
			{
				return { m_PackedValArr.begin() + m_SparseArr[element], false};
			}
			
			emplace(element, std::forward<Args>(args)...);
			return { Iterator{ m_PackedValArr.end() - 1 }, true };
		}
		
		//Do not erase an element that does not exist, use remove instead if this is a concern.
		void erase(KeyType element) noexcept
		{
			ASSERT(contains(element), "Element not in set!");

			//const auto last{ m_DenseArr.back() };

			if constexpr(Impl::MoveAssignmentVal<Val>)
			{
				m_PackedValArr[m_SparseArr[element]] = std::move(m_PackedValArr.back());
			}
			else if constexpr(Impl::MoveConstructVal<Val>)
			{
				m_PackedValArr[m_SparseArr[element]].~Val();
				new (&m_PackedValArr[m_SparseArr[element]]) Val(std::move(m_PackedValArr.back()));
			}

			m_DenseArr[m_SparseArr[element]] = m_DenseArr.back();

			//std::swap(m_SparseArr[last], m_SparseArr[element]);
			m_SparseArr[element] = INVALID_INDEX;
			
			m_DenseArr.pop_back();
			m_PackedValArr.pop_back();
		}

		bool remove(KeyType element) noexcept
		{
			return contains(element) && (erase(element), true);
		}

	private:
		static constexpr KeyType INVALID_INDEX = std::numeric_limits<KeyType>::max();

		std::vector<KeyType> m_SparseArr{ };

		std::vector<KeyType> m_DenseArr{ };
		std::vector<Val> m_PackedValArr{ };
	};
}

#endif