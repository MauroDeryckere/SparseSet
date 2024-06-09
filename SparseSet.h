#ifndef SPARSE_SET
#define SPARSE_SET

#include <vector>

#include <cassert>
#include <string>

#include <type_traits>
#include <limits>

namespace Internal
{
	namespace Impl
	{
		template<typename T>
		concept KeyType = requires
		{
			std::is_integral_v<T> && std::is_unsigned_v<T>;
			std::numeric_limits<T>::max();
		};
	}

	template<typename Val, Impl::KeyType KeyType = uint32_t>
	class SparseSet final
	{
	public:
		SparseSet() = default;
		SparseSet(KeyType reserveSize) :
			m_SparseArr(reserveSize, INVALID_INDEX)
		{ }

		~SparseSet() = default;

		[[nodiscard]] size_t Size() const noexcept { return m_DenseArr.size(); }
		[[nodiscard]] size_t SparseSize() const noexcept { return m_SparseArr.size(); }

		[[nodiscard]] bool Contains(KeyType element) const noexcept { return element < m_SparseArr.size() && m_SparseArr[element] != INVALID_INDEX; }

		template<typename... Args>
		requires std::is_constructible_v<Val, Args...>
		Val& Emplace(KeyType element, Args&&... args) noexcept
		{
			assert(!Contains(element));

			if (element >= m_SparseArr.size())
			{
				m_SparseArr.resize(element + 1, INVALID_INDEX);
			}

			m_SparseArr[element] = static_cast<KeyType>(m_DenseArr.size());

			m_DenseArr.emplace_back(element);
			return m_PackedValArr.emplace_back(std::forward<Args>(args)...);
		}

		//bool Remove(KeyType element) noexcept
		//{

		//}

	private:
		static constexpr KeyType INVALID_INDEX = std::numeric_limits<KeyType>::max();

		std::vector<KeyType> m_SparseArr{ };

		std::vector<KeyType> m_DenseArr{ };
		std::vector<Val> m_PackedValArr{ };
	};
}

#endif