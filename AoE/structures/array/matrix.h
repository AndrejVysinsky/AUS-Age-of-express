#pragma once

#include "../structure.h"
#include "../vector/vector.h"

namespace structures 
{
	
	/// <summary> Matica. </summary>
	/// <typeparam name = "T"> Typ dat ukladanych v matici. </typeparam>
	template<typename T> 
	class Matrix : public Structure
	{
	public:
		/// <summary> Konstruktor vytvori maticu o velkosti [rows x cols] prvkov. </summary>
		/// <param name = "rows"> Pocet riadkov vo vytvaranej matici. </param>
		/// <param name = "cols"> Pocet stlpcov vo vytvaranej matici. </param>
		Matrix(const size_t rows, const size_t cols);

		/// <summary> Kopirovaci konstruktor. </summary>
		/// <param name = "other"> Matica, z ktorej sa prevezmu vlastnosti. </param>
		Matrix(const Matrix<T>& other);

		/// <summary> Destruktor. </summary>
		~Matrix();
		
		/// <summary> Operacia klonovania. Vytvori a vrati duplikat matice. </summary>
		/// <returns> Ukazovatel na vytvoreny klon matice. </returns>
		Structure* clone() const override;

		/// <summary> Vrati pocet prvkov v matici. </summary>
		/// <returns> Pocet prvkov v matici. </returns>
		size_t size() const override;

		/// <summary> Operator priradenia. </summary>
		/// <param name = "other"> Struktura (matica), z ktorej ma prebrat vlastnosti. </param>
		/// <returns> Adresa, na ktorej sa tato matica nachadza po priradeni. </returns>
		/// <exception cref="std::logic_error"> Vyhodena, ak vstupny parameter nie je Matrix. </exception>  
		/// <remarks> Vyuziva typovy operator priradenia. </remarks>
		Structure& operator=(const Structure& other) override;

		/// <summary> Operator priradenia. </summary>
		/// <param name = "other"> Matica, z ktorej ma prebrat vlastnosti. </param>
		/// <returns> Adresa, na ktorej sa tato matica nachadza po priradeni. </returns>
		Matrix<T>& operator=(const Matrix<T>& other);

		/// <summary> Operator porovnania. </summary>
		/// <param name = "other"> Matica, s ktorou sa ma porovnat. </param>
		/// <returns> true, ak su matice rovnake, false inak. </returns>
		bool operator==(const Matrix<T>& other) const;
		
		/// <summary> Vrati adresou prvok na indexe. </summary>
		/// <param name = "row"> Riadkovy index prvku. </param>
		/// <param name = "col"> Stlpcovy index prvku. </param>
		/// <returns> Adresa prvku na danom indexe. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak indexy nepatria do matice. </exception>  
		T& get(const int row, const int col);

		/// <summary> Vrati hodnotou prvok na indexe. </summary>
		/// <param name = "row"> Riadkovy index prvku. </param>
		/// <param name = "col"> Stlpcovy index prvku. </param>
		/// <returns> Hodnota prvku na danom indexe. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak indexy nepatria do matice. </exception>  
		const T get(const int row, const int col) const;

		/// <summary> Skopiruje oblast o velkosti [numRows x numCols] z matice src od pozicie [srcStartRowIndex, srcStartColIndex] do matice dest od pozicie [destStartRowIndex, destStartColIndex]. </summary>
		/// <param name = "src"> Zdrojova matica. </param>
		/// <param name = "srcStartRowIndex"> Index riadku v matici src, od ktoreho sa zacne citat. </param>
		/// <param name = "srcStartColIndex"> Index stlpca v matici src, od ktoreho sa zacne citat. </param>
		/// <param name = "dest"> Cielova matica. </param>
		/// <param name = "destStartRowIndex"> Index riadku v matici dest, od ktoreho sa zacne zapisovat. </param>
		/// <param name = "destStartColIndex"> Index stlpca v matici dest, od ktoreho sa zacne zapisovat. </param>
		/// <param name = "numRows"> Pocet riadkov, ktore budu skopirovane z matice src (zo spravneho miesta) do matice dest (na spravne mieste). </param>
		/// <param name = "numCols"> Pocet stlpcov, ktore budu skopirovane z matice src (zo spravneho miesta) do matice dest (na spravne mieste). </param>
		/// <exception cref="std::out_of_range"> Vyhodena, ak zaciatocne alebo koncove indexy nepatria do prislusnych matic. </exception> 
		static void copy(const Matrix<T>& src,  const int srcStartRowIndex,  const int srcStartColIndex, Matrix<T>& dest, const int destStartRowIndex, const int destStartColIndex, const int numRows, const int numCols);

	private:
		size_t rows_;
		size_t cols_;
		Vector* vector_;
	private:
		/// <summary> Mapovacia funkcia, ktora prevedie index v matici na index vo vektore. </summary>
		/// <param name = "index"> index pozostavajuci z riadkov a stlpcov v matici. </param>
		/// <returns> Index vo vektore. </returns>
		int mapFunction(const int index) const;
	};

	template<typename T>
	Matrix<T>::Matrix(const size_t rows, const size_t cols):
		cols_(cols),
		rows_(rows),
		vector_(new Vector(rows * cols * sizeof(T)))
	{
	}

	template<typename T>
	Matrix<T>::Matrix(const Matrix<T>& other) :
		vector_(new Vector(*(other.vector_))),
		cols_(other.cols_),
		rows_(other.rows_)
	{
	}

	template<typename T>
	Matrix<T>::~Matrix()
	{
		delete vector_;
		this->cols_ = 0;
		this->rows_ = 0;
	}

	template<typename T>
	Structure* Matrix<T>::clone() const
	{
		return new Matrix<T>(*this);
	}

	template<typename T>
	Structure& Matrix<T>::operator=(const Structure& other)
	{
		if (this != &other)
		{
			*this = dynamic_cast<const Matrix<T>&>(other);
		}
		return *this;
	}

	template<typename T>
	Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other)
	{
		if (this != &other) {
			this->rows_ = other.rows_;
			this->cols_ = other.cols_;
			*vector_ = *(other.vector_);
		}
		return *this;
	}
	
	template<typename T>
	size_t Matrix<T>::size() const
	{
		return (this->cols_ * this->rows_);
	}

	template<typename T>
	T& Matrix<T>::get(const int row, const int col)
	{
		return *(T*)(vector_->getBytePointer(mapFunction(row * this->cols_ + col)));
	}

	template<typename T>
	const T Matrix<T>::get(const int row, const int col) const
	{
		return *(T*)(vector_->getBytePointer(mapFunction(row * this->cols_ + col)));
	}

	template<typename T>
	inline bool Matrix<T>::operator==(const Matrix<T>& other) const
	{             
		return (*vector_ == *other.vector_);
	}

	template<typename T>
	void Matrix<T>::copy(const Matrix<T>& src,  const int srcStartRowIndex,  const int srcStartColIndex, Matrix<T>& dest, const int destStartRowIndex, const int destStartColIndex, const int numRows, const int numCols)
	{
		Vector::copy(*src.vector_, src.mapFunction(srcStartRowIndex * src.cols_ + srcStartColIndex), *dest.vector_, dest.mapFunction(destStartRowIndex * dest.cols_ + destStartColIndex), src.mapFunction(numRows * src.cols_ + numCols));
	}

	template<typename T>
	inline int Matrix<T>::mapFunction(const int index) const
	{
		return (sizeof(T) * index);
	}

}

