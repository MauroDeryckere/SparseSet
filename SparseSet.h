#ifndef SPARSE_SET
#define SPARSE_SET

#include <vector>

#include <type_traits>
#include <limits>
#include <utility>
#include <stdexcept>
#include <string>

#include "InternalAssert.h"

namespace Internal
{
	namespace Impl
	{
		template<typename T>
		concept KeyType = std::is_integral_v<T> && std::is_unsigned_v<T> 
						&& std::is_trivially_copyable_v<T> && std::is_swappable_v<T>; //erase function

		template<typename T>
		concept MoveAssignmentVal = std::is_nothrow_move_assignable_v<T>;

		//We only want to move construct when we arent able to move assign
		template<typename T>
		concept MoveConstructVal = !MoveAssignmentVal<T> && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>;

		template<typename T>
		concept ValType =  (std::is_trivially_copyable_v<T> || MoveAssignmentVal<T> || MoveConstructVal<T>);
	}

	template<Impl::KeyType KeyType>
	class sparse_set_out_of_range : public std::out_of_range
	{
	public:
		sparse_set_out_of_range(std::string_view message, KeyType element) :
			std::out_of_range{ message.data() },
			m_Element{ element }
		{ }

		~sparse_set_out_of_range() = default;

		[[nodiscard]] KeyType element() const noexcept
		{
			return m_Element;
		}

	private:
		const KeyType m_Element;
	};

	template<Impl::ValType Val, Impl::KeyType KeyType = uint32_t>
	class sparse_set final
	{
	public:
		sparse_set() noexcept = default;

		sparse_set(std::initializer_list<std::pair<KeyType, Val&&>> initList, KeyType reserveSize = 0) noexcept
		{
			sparse_reserve(static_cast<KeyType>(initList.size()));
			reserve(reserveSize);

			for (auto&& pair : initList)
			{
				emplace(pair.first, std::forward<Val>(pair.second));
			}
		}

		sparse_set(KeyType sparseSize, KeyType reserveSize = 0) noexcept :
			m_SparseArr(sparseSize, INVALID_INDEX)
		{ 
			reserve(reserveSize);
		}

		~sparse_set() noexcept = default;

		sparse_set(const sparse_set& other) noexcept :
			m_SparseArr{ other.m_SparseArr },
			m_PackedValArr{ other.m_PackedValArr },
			m_DenseArr{ other.m_DenseArr }
		{ }

		sparse_set& operator=(const sparse_set& other) noexcept
		{
			m_SparseArr = other.m_SparseArr;
			m_PackedValArr = other.m_PackedValArr;
			m_DenseArr = other.m_DenseArr;

			return *this;
		}

		sparse_set(sparse_set&& other) noexcept :
			m_SparseArr{ std::move(other.m_SparseArr) },
			m_PackedValArr{ std::move(other.m_PackedValArr) },
			m_DenseArr{ std::move(other.m_DenseArr) }
		{ }

		sparse_set& operator=(sparse_set&& other) noexcept 
		{
			m_SparseArr = std::move(other.m_SparseArr);
			m_PackedValArr = std::move(other.m_PackedValArr);
			m_DenseArr = std::move(other.m_DenseArr);

			return *this;
		}

		void swap(sparse_set& other) noexcept
		{
			std::swap(m_SparseArr, other.m_SparseArr);
			std::swap(m_DenseArr, other.m_DenseArr);
			std::swap(m_PackedValArr, other.m_PackedValArr);
		}

	public:
		[[nodiscard]] size_t size() const noexcept { return m_DenseArr.size(); }
		[[nodiscard]] size_t sparse_size() const noexcept { return m_SparseArr.size(); }

		[[nodiscard]] static constexpr KeyType max_sparse_size() noexcept
		{
			return INVALID_INDEX - 1;
		}

		void resize(KeyType newSize, KeyType reserveSize = 0) noexcept
		{
			ASSERT(newSize > m_SparseArr.size(), "");

			m_SparseArr.resize(newSize, INVALID_INDEX);
			m_DenseArr.reserve(reserveSize);
			m_PackedValArr.reserve(reserveSize);
		}

		void shrink_to_fit() noexcept
		{
			m_SparseArr.shrink_to_fit();
			m_DenseArr.shrink_to_fit();
			m_PackedValArr.shrink_to_fit();
		}

		void sparse_reserve(KeyType newCap) noexcept
		{
			m_SparseArr.reserve(newCap);
		}

		void reserve(KeyType newCap) noexcept
		{
			m_DenseArr.reserve(newCap);
			m_PackedValArr.reserve(newCap);
		}

		[[nodiscard]] bool empty() const noexcept { return m_DenseArr.empty(); }

		void clear() noexcept
		{
			m_DenseArr.clear();
			m_PackedValArr.clear();
			m_SparseArr.clear();
		}

		using key_type = KeyType;
		using dense_type = KeyType;
		using value_type = Val;

		using iterator = typename std::vector<Val>::iterator;
		using const_iterator = typename std::vector<Val>::const_iterator;
		
		using reverse_iterator = typename std::vector<Val>::reverse_iterator;
		using const_reserve_iterator = typename std::vector<Val>::const_reverse_iterator;

		iterator begin() noexcept { return m_PackedValArr.begin(); }
		iterator end() noexcept { return m_PackedValArr.end(); }
		const_iterator begin() const noexcept { return m_PackedValArr.begin(); }
		const_iterator end() const noexcept { return m_PackedValArr.end(); }

		reverse_iterator rbegin() noexcept { return m_PackedValArr.rbegin(); }
		reverse_iterator rend() noexcept { return m_PackedValArr.rend(); }
		const_reserve_iterator rbegin() const noexcept { return m_PackedValArr.rbegin(); }
		const_reserve_iterator rend() const noexcept { return m_PackedValArr.rend(); }

		const_iterator cbegin() const noexcept { return m_PackedValArr.cbegin(); }
		const_iterator cend() const noexcept { return m_PackedValArr.cend(); }
		const_reserve_iterator crbegin() const noexcept { return m_PackedValArr.crbegin(); }
		const_reserve_iterator crend() const noexcept { return m_PackedValArr.crend(); }

	public:
		[[nodiscard]] bool contains(KeyType element) const noexcept { return element < m_SparseArr.size() && m_SparseArr[element] != INVALID_INDEX; }

		//Iterator must be in bounds to get a valid value
		template <typename IteratorType>
		[[nodiscard]] KeyType sparse_index(IteratorType it) const noexcept
		{
			ASSERT(contains(m_DenseArr[val_index(it)]), "");
			return m_DenseArr[val_index(it)];
		}
		
		//Element must exist to get a valid value
		Val& operator[](KeyType element) noexcept
		{
			ASSERT(contains(element), "Element not in set!");
			return m_PackedValArr[m_SparseArr[element]];
		}
		const Val& operator[](KeyType element) const noexcept
		{
			ASSERT(contains(element), "Element not in set!");
			return m_PackedValArr[m_SparseArr[element]];
		}

		//Random access with bounds checking (similar to std::vector:::at())
		Val& at(KeyType element)
		{
			if (contains(element))
			{
				return m_PackedValArr[m_SparseArr[element]];
			}
			throw sparse_set_out_of_range( "Element not found in sparse_set", element );
		}
		const Val& at(KeyType element) const
		{
			if (contains(element))
			{
				return m_PackedValArr[m_SparseArr[element]];
			}
			throw sparse_set_out_of_range( "Element not found in sparse_set", element );
		}

		const_iterator find(KeyType key) const noexcept
		{
			if (contains(key))
			{
				return m_PackedValArr.cbegin() + m_SparseArr[key];
			}
			return m_PackedValArr.cend();
		}		
		iterator find(KeyType key) noexcept
		{
			if (contains(key))
			{
				return m_PackedValArr.begin() + m_SparseArr[key];
			}
			return m_PackedValArr.end();
		}

	public:
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
		std::pair<iterator, bool> try_emplace(KeyType element, Args&&... args) noexcept
		{
			if (contains(element))
			{
				return { m_PackedValArr.begin() + m_SparseArr[element], false};
			}
			
			emplace(element, std::forward<Args>(args)...);
			return { iterator{ m_PackedValArr.end() - 1 }, true };
		}

		template<typename... Args>
		requires std::is_constructible_v<Val, Args...>
		Val& get_or_emplace(KeyType element, Args&&... args) noexcept
		{
			if (!contains(element))
			{
				return emplace(element, std::forward<Args>(args)...);
			}

			return m_PackedValArr[m_SparseArr[element]];
		}

	public:
		//Do not erase an element that does not exist, use remove instead if this is a concern.
		void erase(KeyType element) noexcept
		{
			ASSERT(contains(element), "Element not in set!");

			if constexpr (std::is_trivially_copyable_v<Val>)
			{
				std::memcpy(&m_PackedValArr[m_SparseArr[element]], &m_PackedValArr.back(), sizeof(Val));
			}
			else if constexpr(Impl::MoveAssignmentVal<Val>)
			{
				m_PackedValArr[m_SparseArr[element]] = std::move(m_PackedValArr.back());
			}
			else if constexpr(Impl::MoveConstructVal<Val>)
			{
				m_PackedValArr[m_SparseArr[element]].~Val();
				new (&m_PackedValArr[m_SparseArr[element]]) Val(std::move(m_PackedValArr.back()));
			}

			m_DenseArr[m_SparseArr[element]] = m_DenseArr.back();

			m_SparseArr[m_DenseArr.back()] = element;
			m_SparseArr[element] = INVALID_INDEX;

			m_DenseArr.pop_back();
			m_PackedValArr.pop_back();
		}

		//Do not erase with iterator that's out of bounds
		iterator erase(const_iterator pos) noexcept
		{
			ASSERT(!(pos >= end() && pos < begin()), "Iterator out of bounds!");

			const auto distance{ std::distance(cbegin(), pos) };
			erase(m_DenseArr[val_index(pos)]);

			return begin() + distance;
		}

		//Do not erase with iterator range that's out of bounds
		iterator erase(const_iterator first, const_iterator last) noexcept
		{
			ASSERT(!(last > cend() || first < cbegin()) && first < last, "Iterator out of bounds!");
			ASSERT(first != last, "First == last erases nothing!");

			--last;
			while (last > first)
			{
				last = --erase(last);
			}

			return erase(last);
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

	private:
		template <typename IteratorType>
		[[nodiscard]] KeyType val_index(IteratorType pos) const noexcept
		{
			if constexpr (std::is_same_v<IteratorType, reverse_iterator>
				|| std::is_same_v<IteratorType, const_reserve_iterator>)
			{
				ASSERT(!(pos >= rend() && pos < rbegin()), "Reverse iterator out of bounds");
				return static_cast<KeyType>(rend() - 1 - pos);
			}
			else if constexpr (std::is_same_v<IteratorType, iterator>
					|| std::is_same_v<IteratorType, const_iterator>)
			{
				ASSERT(!(pos >= end() && pos < begin()), "Iterator out of bounds");
				return static_cast<KeyType>(pos - begin());
			}
		}
	};
}

#endif