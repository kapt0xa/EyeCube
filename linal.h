#pragma once
#include <array>
#include <cmath>
#include <cassert>

namespace cube_game
{
	template<typename ScalarT, size_t dimentions>
	struct Vector : public std::array<ScalarT, dimentions>
	{
		using Vector::array::array;

		Vector(const ScalarT& x = {}, const ScalarT& y = {}, const ScalarT& z = {}, const ScalarT& w = {});
		bool operator == (const Vector& right) const;
		bool operator != (const Vector& right) const;
		size_t Hash() const;

		ScalarT& X();
		const ScalarT& X() const;
		ScalarT& Y();
		const ScalarT& Y() const;
		ScalarT& Z();
		const ScalarT& Z() const;
		ScalarT& W();
		const ScalarT& W() const;
		// immaginary part of Quaternion
		Vector<ScalarT, 3>& QIm();
		// immaginary part of Quaternion
		const Vector<ScalarT, 3>& QIm() const;

		Vector& operator *= (const ScalarT& scalar);
		Vector& operator /= (const ScalarT& scalar);
		Vector& operator += (const Vector& right);
		Vector& operator -= (const Vector& right);

		// xyz >> 1 = zxy
		Vector operator >> (ptrdiff_t offset) const;
		// xyz << 1 = yzx
		Vector operator << (ptrdiff_t offset) const;

		Vector operator * (const ScalarT& scalar) const;
		Vector operator / (const ScalarT& scalar) const;
		Vector operator + (const Vector& right) const;
		Vector operator - (const Vector& right) const;

		Vector operator- () const;
		const Vector& operator+ () const;
		// 2d and 4d only. returns 1/quat or 1/complex
		Vector operator~ () const;

		// 2d or 4d only. threated as complex number
		Vector Conjugated() const;
		// 0-4 d only. 1d: usual scalar multyply, 2d - complex multiply, 3d - cross product, 4d - quaternion multyply
		Vector operator * (const Vector& right) const;
		// 1,2 d only. threated as scalar or complex number
		Vector operator / (const Vector& right) const;
		ScalarT Dot(const Vector& right) const;
		// 3d odly
		Vector Cross(const Vector& right) const;
		ScalarT Magnitude2() const;
		ScalarT Magnitude() const;
		Vector& Normalize();
		Vector Normalized() const;

		template<typename OtherScalarT>
		Vector<OtherScalarT, dimentions> Cast() const;
		template<typename OtherScalarT>
		explicit operator Vector<OtherScalarT, dimentions>() const;

		template<size_t other_dimentions>
		Vector<ScalarT, other_dimentions> Resized() const;
		template<size_t other_dimentions>
		explicit operator Vector<ScalarT, other_dimentions>() const;
		template<size_t other_dimentions>
		const Vector<ScalarT, other_dimentions>& Prefix() const;
		template<size_t other_dimentions>
		Vector<ScalarT, other_dimentions>& Prefix();

		// only for 2d and 4d complex numbers
		static Vector Identity();

		static Vector Zero();
		static Vector Ones();
		template<size_t dimention>
		static Vector Basis();
		static Vector Basis(size_t dimention);
		static Vector I();
		static Vector J();
		static Vector K();
		static Vector L();
	};

	using V2f = Vector<float, 2>;
	using V3f = Vector<float, 3>;
	using V4f = Vector<float, 4>;
	using Quat = Vector<float, 4>;
	using V2i = Vector<int, 2>;
	using V3i = Vector<int, 3>;
	using V4i = Vector<int, 4>;

	template<typename ScalarT, size_t dimentions>
	struct Matrix : public Vector<Vector<ScalarT, dimentions>, dimentions>
	{
		using Matrix::Vector::Vector;
		using Index2 = Vector<ptrdiff_t, 2>;
		using UIndex2 = Vector<size_t, 2>;

		static Matrix Identity();

		Matrix operator * (const Matrix& right) const;
		Matrix& operator *= (const ScalarT& right);
		Matrix& operator /= (const ScalarT& right);
		Matrix operator * (const ScalarT& right) const;
		Matrix operator / (const ScalarT& right) const;
		Vector<ScalarT, dimentions> operator * (const Vector<ScalarT, dimentions>& right) const;
		ScalarT Det() const;
		Matrix Transponeered() const;

		Matrix operator << (Index2 offset) const;
		Matrix operator >> (Index2 offset) const;

		template<size_t other_dimentions>
		Matrix<ScalarT, other_dimentions> Resized() const;

		Vector<Vector<ScalarT, dimentions>, dimentions>& AsVect();
		const Vector<Vector<ScalarT, dimentions>, dimentions>& AsVect() const;
	};
	using M2f = Matrix<float, 2>;
	using M3f = Matrix<float, 3>;
	using M4f = Matrix<float, 4>;
	using M2i = Matrix<int, 2>;
	using M3i = Matrix<int, 3>;
	using M4i = Matrix<int, 4>;
}

namespace std
{
	template<typename ScalarT, size_t dimentions>
	struct hash<cube_game::Vector<ScalarT,dimentions>>
	{
		size_t operator()(const cube_game::Vector<ScalarT, dimentions>& v) const
		{
			size_t hash = 0;
			for (size_t i = 0; i != v.size(); ++i)
			{
				constexpr int shifted_golden_ratio = 0x9e3779b9;
				hash ^= std::hash<ScalarT>()(v[i]) + shifted_golden_ratio + (hash << 6) + (hash >> 2); // provided by chat GPT, keine idee if it is good hashing.
			}
			return hash;
		}
	};
}

namespace cube_game
{
	template<typename ScalarT, size_t dimentions>
	using VectorHasher = std::hash<Vector<ScalarT, dimentions>>;
	using HV2f = VectorHasher<float, 2>;
	using HV3f = VectorHasher<float, 3>;
	using HV4f = VectorHasher<float, 4>;
	using HV2i = VectorHasher<int, 2>;
	using HV3i = VectorHasher<int, 3>;
	using HV4i = VectorHasher<int, 4>;

	// vvv implementations vvv ====================================

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions>::Vector(const ScalarT& x, const ScalarT& y, const ScalarT& z, const ScalarT& w)
	{
		if constexpr (dimentions >= 1)
		{
			X() = x;
		}
		if constexpr (dimentions >= 2)
		{
			Y() = y;
		}
		if constexpr (dimentions >= 3)
		{
			Z() = z;
		}
		if constexpr (dimentions >= 4)
		{
			W() = w;
		}
	}

	template<typename ScalarT, size_t dimentions>
	inline bool Vector<ScalarT, dimentions>::operator==(const Vector& right) const
	{
		for (size_t i = 0; i != this->size(); ++i)
		{
			if ((*this)[i] != right[i])
			{
				return false;
			}
		}
		return true;
	}

	template<typename ScalarT, size_t dimentions>
	inline bool Vector<ScalarT, dimentions>::operator!=(const Vector& right) const
	{
		return !(*this == right);
	}

	template<typename ScalarT, size_t dimentions>
	inline size_t Vector<ScalarT, dimentions>::Hash() const
	{
		return std::hash<Vector>()(*this);
	}

	template<typename ScalarT, size_t dimentions>
	inline ScalarT& Vector<ScalarT, dimentions>::X()
	{
		static_assert(dimentions >= 1, "not enought dimentions");
		return (*this)[0];
	}
	template<typename ScalarT, size_t dimentions>
	inline const ScalarT& Vector<ScalarT, dimentions>::X() const
	{
		static_assert(dimentions >= 1, "not enought dimentions");
		return (*this)[0];
	}

	template<typename ScalarT, size_t dimentions>
	inline ScalarT& Vector<ScalarT, dimentions>::Y()
	{
		static_assert(dimentions >= 2, "not enought dimentions");
		return (*this)[1];
	}
	template<typename ScalarT, size_t dimentions>
	inline const ScalarT& Vector<ScalarT, dimentions>::Y() const
	{
		static_assert(dimentions >= 2, "not enought dimentions");
		return (*this)[1];
	}

	template<typename ScalarT, size_t dimentions>
	inline ScalarT& Vector<ScalarT, dimentions>::Z()
	{
		static_assert(dimentions >= 3, "not enought dimentions");
		return (*this)[2];
	}
	template<typename ScalarT, size_t dimentions>
	inline const ScalarT& Vector<ScalarT, dimentions>::Z() const
	{
		static_assert(dimentions >= 3, "not enought dimentions");
		return (*this)[2];
	}

	template<typename ScalarT, size_t dimentions>
	inline ScalarT& Vector<ScalarT, dimentions>::W()
	{
		static_assert(dimentions >= 4, "not enought dimentions");
		return (*this)[3];
	}
	template<typename ScalarT, size_t dimentions>
	inline const ScalarT& Vector<ScalarT, dimentions>::W() const
	{
		static_assert(dimentions >= 4, "not enought dimentions");
		return (*this)[3];
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, 3>& Vector<ScalarT, dimentions>::QIm()
	{
		return Prefix<3>();
	}
	template<typename ScalarT, size_t dimentions>
	inline const Vector<ScalarT, 3>& Vector<ScalarT, dimentions>::QIm() const
	{
		return Prefix<3>();
	}

	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions>& Vector<ScalarT, dimentions>::operator *= (const ScalarT& scalar)
	{
		for (auto& cord : *this)
		{
			cord *= scalar;
		}
		return *this;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions>& Vector<ScalarT, dimentions>::operator /= (const ScalarT& scalar)
	{
		for (auto& cord : *this)
		{
			cord /= scalar;
		}
		return *this;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions>& Vector<ScalarT, dimentions>::operator += (const Vector& right)
	{
		auto it_r = right.begin();
		for (auto it_l = this->begin(); it_l != this->end(); ++it_l, ++it_r)
		{
			*it_l += *it_r;
		}
		return *this;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions>& Vector<ScalarT, dimentions>::operator -= (const Vector& right)
	{
		for (size_t i = 0; i != this->size(); ++i)
		{
			(*this)[i] -= right[i];
		}
		return *this;
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator>>(ptrdiff_t offset) const
	{
		if (offset < 0)
		{
			return *this << -offset;
		}
		Vector shifted;
		for (size_t i = 0; i != this->size(); ++i)
		{
			shifted[(i + offset) % this->size()] = (*this)[i];
		}
		return shifted;
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator<<(ptrdiff_t offset) const
	{
		if (offset < 0)
		{
			return *this << -offset;
		}
		Vector shifted;
		for (size_t i = 0; i != this->size(); ++i)
		{
			shifted[i] = (*this)[(i + offset) % this->size()];
		}
		return shifted;
	}

	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator * (const ScalarT& scalar) const
	{
		return Vector(*this) *= scalar;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator / (const ScalarT& scalar) const
	{
		return Vector(*this) /= scalar;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator + (const Vector& right) const
	{
		return Vector(*this) += right;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator - (const Vector& right) const
	{
		return Vector(*this) -= right;
	}

	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator- () const
	{
		auto negated = *this;
		for (auto& cord : negated)
		{
			cord = -cord;
		}
		return negated;
	}
	template<typename ScalarT, size_t dimentions>
	const Vector<ScalarT, dimentions>& Vector<ScalarT, dimentions>::operator+ () const
	{
		return *this;
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator~() const
	{
		return Conjugated() /= Magnitude2();
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Conjugated() const
	{
		if constexpr (dimentions == 4)
		{
			Vector conjugated;
			conjugated.W() = W();
			conjugated.QIm() = -QIm();
			return conjugated;
		}
		else if constexpr (dimentions == 2)
		{
			Vector conjugated;
			conjugated.X() = X();
			conjugated.Y() = -Y();
			return conjugated;
		}
		else
		{
			static_assert(false, "conjugated existd only for 2d- and 4d-complex numbers");
		}
	}

	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator * (const Vector& right) const
	{
		if constexpr (dimentions == 0) // no change
		{
			return *this;
		}
		else if constexpr (dimentions == 1) // threated as scalar multiplication
		{
			return *this * right.X();
		}
		else if constexpr (dimentions == 2) // threated as 2d complex multiplication
		{
			return
				Vector(
					X() * right.X() - Y() * right.Y(),
					X() * right.Y() + Y() * right.X()
				);
		}
		else if constexpr (dimentions == 3) // threated as 3d cross product
		{
			return Cross(right);
		}
		else if constexpr (dimentions == 4) // threated as quaternion(4d-complex) multiplication
		{
			Vector result;
			result.QIm() =
				QIm() * right.W() +
				right.QIm() * W() +
				QIm() * right.QIm();// finish quat * quat
			result.W() = right.W() * W() - QIm().Dot(right.QIm());
			return result;
		}
		else
		{
			static_assert(false, "multiplication for multidimentional vectors is not implemented (more than 4 dimentions)");
		}
	}
	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::operator/(const Vector& right) const
	{
		if constexpr (dimentions == 1)
		{
			return *this / right.X();
		}
		else if constexpr (dimentions == 2)
		{
			return *this * right.Conjugated() / right.Magnitude2();
		}
		else
		{
			static_assert(false, "division is defined only for scalars and 2d-complex numbers");
		}
	}
	template<typename ScalarT, size_t dimentions>
	ScalarT Vector<ScalarT, dimentions>::Dot(const Vector<ScalarT, dimentions>& right) const
	{
		ScalarT dot_product = {};
		for (size_t i = 0; i != (*this).size(); ++i)
		{
			dot_product += (*this)[i] * right[i];
		}
		return dot_product;
	}
	template<typename ScalarT, size_t dimentions>
	Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Cross(const Vector<ScalarT, dimentions>& right) const
	{
		static_assert(dimentions == 3, "cross product exists only for 3-dimentions vectors");
		return Vector(
			Y() * right.Z() - Z() * right.Y(),
			Z() * right.X() - X() * right.Z(),
			X() * right.Y() - Y() * right.X()
		);
	}
	template<typename ScalarT, size_t dimentions>
	inline ScalarT Vector<ScalarT, dimentions>::Magnitude2() const
	{
		return Dot(*this);
	}
	template<typename ScalarT, size_t dimentions>
	inline ScalarT Vector<ScalarT, dimentions>::Magnitude() const
	{
		return static_cast<ScalarT>(std::sqrt(Magnitude2()));
	}
	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions>& Vector<ScalarT, dimentions>::Normalize()
	{
		return *this /= Magnitude();
	}
	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Normalized() const
	{
		return Vector(*this) /= Magnitude();
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Identity()
	{
		if constexpr (dimentions == 4)
		{
			return L();
		}
		else if constexpr (dimentions == 2)
		{
			return I();
		}
		else
		{
			static_assert(false, "identity exists only for 2d and 4d complex numbers");
		}
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Zero()
	{
		return Vector();
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Ones()
	{
		Vector ones;
		ones.fill({ 1 });
		return ones;
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Basis(size_t dimention)
	{
		assert(dimention < dimentions); // basis dimention should be less than whole vector dimentions count
		auto basis = Vector();
		basis[dimention] = ScalarT{ 1 };
		return basis;
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::I()
	{
		return Vector::template Basis<0>();
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::J()
	{
		return Vector::template Basis<1>();
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::K()
	{
		return Vector::template Basis<2>();
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::L()
	{
		return Vector::template Basis<3>();
	}

	template<typename ScalarT, size_t dimentions>
	template<typename OtherScalarT>
	inline Vector<OtherScalarT, dimentions> Vector<ScalarT, dimentions>::Cast() const
	{
		Vector<OtherScalarT, dimentions> casted;
		for (size_t i = 0; i != this->size(); ++i)
		{
			casted[i] = static_cast<OtherScalarT>((*this)[i]);
		}
		return casted;
	}
	template<typename ScalarT, size_t dimentions>
	template<typename OtherScalarT>
	inline Vector<ScalarT, dimentions>::operator Vector<OtherScalarT, dimentions>() const
	{
		return Cast<OtherScalarT>();
	}

	template<typename ScalarT, size_t dimentions>
	template<size_t other_dimentions>
	inline Vector<ScalarT, other_dimentions> Vector<ScalarT, dimentions>::Resized() const
	{
		Vector<ScalarT, other_dimentions> resized;
		for (size_t i = 0; (i != (*this).size()) && (i != resized.size()); ++i)
		{
			resized[i] = (*this)[i];
		}
		for (size_t i = (*this).size(); i < resized.size(); ++i)
		{
			resized[i] = ScalarT{};
		}
		return resized;
	}
	template<typename ScalarT, size_t dimentions>
	template<size_t other_dimentions>
	inline Vector<ScalarT, dimentions>::operator Vector<ScalarT, other_dimentions>() const
	{
		return Resized<other_dimentions>();
	}
	template<typename ScalarT, size_t dimentions>
	template<size_t other_dimentions>
	const Vector<ScalarT, other_dimentions>& Vector<ScalarT, dimentions>::Prefix() const
	{
		static_assert(other_dimentions <= dimentions, "prefix should be less than whole vector");
		return reinterpret_cast<const Vector<ScalarT, other_dimentions>&>(*this);
	}
	template<typename ScalarT, size_t dimentions>
	template<size_t other_dimentions>
	Vector<ScalarT, other_dimentions>& Vector<ScalarT, dimentions>::Prefix()
	{
		static_assert(other_dimentions <= dimentions, "prefix should be less than whole vector");
		return reinterpret_cast<Vector<ScalarT, other_dimentions>&>(*this);
	}
	template<typename ScalarT, size_t dimentions>
	template<size_t dimention>
	inline Vector<ScalarT, dimentions> Vector<ScalarT, dimentions>::Basis()
	{
		static_assert(dimention < dimentions, "basis dimention should be less than whole vector dimentions count");
		auto basis = Vector();
		basis[dimention] = ScalarT{ 1 };
		return basis;
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::Identity()
	{
		Matrix identity;
		for (size_t i = 0; i != dimentions; ++i)
		{
			identity[i][i] = { 1 };
		}
		return identity;
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::operator*(const Matrix& right) const
	{
		auto rt = right.Transponeered();
		Matrix product;
		for (size_t i = 0; i != dimentions; ++i)
		{
			product[i] = (*this)*rt[i];
		}

		return product;
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions>& Matrix<ScalarT, dimentions>::operator*=(const ScalarT& right)
	{
		for (auto& vect : *this)
		{
			vect *= right;
		}
		return *this;
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions>& Matrix<ScalarT, dimentions>::operator/=(const ScalarT& right)
	{
		for (auto& vect : *this)
		{
			vect /= right;
		}
		return *this;
	}
	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::operator*(const ScalarT& right) const
	{
		return Matrix(*this) *= right;
	}
	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::operator/(const ScalarT& right) const
	{
		return Matrix(*this) /= right;
	}
	template<typename ScalarT, size_t dimentions>
	inline Vector<ScalarT, dimentions> Matrix<ScalarT, dimentions>::operator*(const Vector<ScalarT, dimentions>& right) const
	{
		Vector<ScalarT, dimentions> result;
		for (size_t i = 0; i != dimentions; ++i)
		{
			result[i] = (*this)[i].Dot(right);
		}
		return result;
	}
	template<typename ScalarT, size_t dimentions>
	inline ScalarT Matrix<ScalarT, dimentions>::Det() const
	{
		if constexpr (dimentions == 0)
		{
			return 0;
		}
		else if constexpr (dimentions == 1)
		{
			return (*this)[0][0];
		}
		else if constexpr (dimentions == 2)
		{
			return
				(*this)[0][0] *
				(*this)[1][1]
				+
				(*this)[0][1] *
				(*this)[1][0];
		}
		else if constexpr (dimentions == 3)
		{
			return (*this)[0].Cross((*this)[1]).Dot((*this)[2]);
		}
		else
		{
			ScalarT det = {};
			for (size_t i = 0; i != dimentions; ++i)
			{
				auto minor = (*this)<<Index2(i).Resized<dimentions-1>();
				det += (*this)[0][i] * minor.Det() *
					(i % 2 ? ScalarT{ -1 } : ScalarT{ 1 });
			}
			return det;
		}
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::Transponeered() const
	{
		Matrix transponeered;
		for (size_t i = 0; i != dimentions; ++i)
			for (size_t j = 0; j != dimentions; ++j)
			{
				transponeered[i][j] = (*this)[j][i];
			}
		return transponeered;
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::operator<<(Index2 offset) const
	{
		Matrix shifted;
		shifted.AsVect() = AsVect() << offset.Y();
		for (size_t i = 0; i != dimentions; ++i)
		{
			shifted[i] = shifted[i] << offset.X();
		}
		return shifted;
	}

	template<typename ScalarT, size_t dimentions>
	inline Matrix<ScalarT, dimentions> Matrix<ScalarT, dimentions>::operator>>(Index2 offset) const
	{
		return *this << -offset;
	}

	template<typename ScalarT, size_t dimentions>
	inline Vector<Vector<ScalarT, dimentions>, dimentions>& Matrix<ScalarT, dimentions>::AsVect()
	{
		return *this;
	}

	template<typename ScalarT, size_t dimentions>
	inline const Vector<Vector<ScalarT, dimentions>, dimentions>& Matrix<ScalarT, dimentions>::AsVect() const
	{
		return *this;
	}

	template<typename ScalarT, size_t dimentions>
	template<size_t other_dimentions>
	inline Matrix<ScalarT, other_dimentions> Matrix<ScalarT, dimentions>::Resized() const
	{
		Matrix<ScalarT, other_dimentions> resized;
		for (size_t i = 0; (i != dimentions) && (i != other_dimentions); ++i)
		{
			resized[i] = (*this)[i].template Resized<other_dimentions>();
		}
		for (size_t i = dimentions; i < other_dimentions; ++i)
		{
			resized[i] = Vector<ScalarT, other_dimentions>{};
			resized[i][i] = { 1 };
		}
		return resized;
	}
}

