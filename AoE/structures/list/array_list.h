#pragma once

#include "list.h"
#include "../structure_iterator.h"
#include "../array/array.h"

namespace structures
{

	/// <summary> Zoznam implementovany polom. </summary>
	/// <typeparam name = "T"> Typ dat ukladanych v zozname. </typepram>
	template<typename T>
	class ArrayList : public List<T>
	{
	public:
		/// <summary> Konstruktor. </summary>
		ArrayList();

		/// <summary> Kopirovaci konstruktor. </summary>
		/// <param name = "other"> ArrayList, z ktoreho sa prevezmu vlastnosti. </param>
		ArrayList(const ArrayList<T>& other);

		/// <summary> Destruktor. </summary>
		~ArrayList();
		
		/// <summary> Operacia klonovania. Vytvori a vrati duplikat zoznamu. </summary>
		/// <returns> Ukazovatel na klon struktury. </returns>
		Structure* clone() const override;

		/// <summary> Vrati pocet prvkov v zozname. </summary>
		/// <returns> Pocet prvkov v zozname. </returns>
		size_t size() const override;

		/// <summary> Operator priradenia. </summary>
		/// <param name = "other"> Zoznam, z ktoreho ma prebrat vlastnosti. </param>
		/// <returns> Adresa, na ktorej sa tento zoznam nachadza po priradeni. </returns>
		List<T>& operator=(const List<T>& other) override;

		/// <summary> Operator priradenia. </summary>
		/// <param name = "other"> Zoznam, z ktoreho ma prebrat vlastnosti. </param>
		/// <returns> Adresa, na ktorej sa tento zoznam nachadza po priradeni. </returns>
		ArrayList<T>& operator=(const ArrayList<T>& other);

		/// <summary> Vrati adresou prvok na indexe. </summary>
		/// <param name = "index"> Index prvku. </param>
		/// <returns> Adresa prvku na danom indexe. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak index nepatri do zoznamu. </exception>  
		T& operator[](const int index) override;

		/// <summary> Vrati hodnotou prvok na indexe. </summary>
		/// <param name = "index"> Index prvku. </param>
		/// <returns> Hodnota prvku na danom indexe. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak index nepatri do zoznamu. </exception>  
		const T operator[](const int index) const override;

		/// <summary> Prida prvok do zoznamu. </summary>
		/// <param name = "data"> Pridavany prvok. </param>
		void add(const T& data) override;

		/// <summary> Vlozi prvok do zoznamu na dany index. </summary>
		/// <param name = "data"> Pridavany prvok. </param>
		/// <param name = "index"> Index prvku. </param>
		/// <exception cref="std::out_of_range"> Vyhodena, ak index nepatri do zoznamu. </exception>  
		/// <remarks> Ak je ako index zadana hodnota poctu prvkov (teda prvy neplatny index), metoda insert sa sprava ako metoda add. </remarks>
		void insert(const T& data, const int index) override;

		/// <summary> Odstrani prvy vyskyt prvku zo zoznamu. </summary>
		/// <param name = "data"> Odstranovany prvok. </param>
		/// <returns> true, ak sa podarilo prvok zo zoznamu odobrat, false inak. </returns>
		bool tryRemove(const T& data) override;

		/// <summary> Odstrani zo zoznamu prvok na danom indexe. </summary>
		/// <param name = "index"> Index prvku. </param>
		/// <returns> Odstraneny prvok. </returns>
		/// <exception cref="std::out_of_range"> Vyhodena, ak index nepatri do zoznamu. </exception>  
		T removeAt(const int index) override;

		/// <summary> Vrati index prveho vyskytu prvku v zozname. </summary>
		/// <param name = "data"> Prvok, ktoreho index sa hlada. </param>
		/// <returns> Index prveho vyskytu prvku v zozname, ak sa prvok v zozname nenachadza, vrati -1. </returns>
		int getIndexOf(const T& data) override;

		/// <summary> Vymaze zoznam. </summary>
		void clear() override;
	
		/// <summary> Vrati skutocny iterator na zaciatok struktury </summary>
		/// <returns> Iterator na zaciatok struktury. </returns>
		/// <remarks> Zabezpecuje polymorfizmus. </remarks>
		Iterator<T>* getBeginIterator() const override;

		/// <summary> Vrati skutocny iterator na koniec struktury </summary>
		/// <returns> Iterator na koniec struktury. </returns>
		/// <remarks> Zabezpecuje polymorfizmus. </remarks>
		Iterator<T>* getEndIterator() const override;
	private:
		/// <summary> Pole s datami. </summary>
		Array<T>* array_;
		/// <summary> Pocet prvkov v zozname. </summary>
		size_t size_;
	private:
		/// <summary> Rozsiri kapacitu zoznamu. </summary>
		void enlarge();

		/// <summary> Iterator pre ArrayList. </summary>
		class ArrayListIterator : public Iterator<T>
		{
		public:
			/// <summary> Konstruktor. </summary>
			/// <param name = "arrayList"> Zoznam, cez ktory iteruje. </param>
			/// <param name = "position"> Pozicia, na ktorej zacina. </param>
			ArrayListIterator(const ArrayList<T>* arrayList, int position);

			/// <summary> Destruktor. </summary>
			~ArrayListIterator();

			/// <summary> Operator priradenia. Priradi do seba hodnotu druheho iteratora. </summary>
			/// <param name = "other"> Druhy iterator. </param>
			/// <returns> Vrati seba po priradeni. </returns>
			Iterator<T>& operator= (const Iterator<T>& other) override;

			/// <summary> Porovna sa s druhym iteratorom na nerovnost. </summary>
			/// <param name = "other"> Druhy iterator. </param>
			/// <returns> True, ak sa iteratory nerovnaju, false inak. </returns>
			bool operator!=(const Iterator<T>& other) override;

			/// <summary> Vrati data, na ktore aktualne ukazuje iterator. </summary>
			/// <returns> Data, na ktore aktualne ukazuje iterator. </returns>
			const T operator*() override;

			/// <summary> Posunie iterator na dalsi prvok v strukture. </summary>
			/// <returns> Iterator na dalsi prvok v strukture. </returns>
			/// <remarks> Zvycajne vrati seba. Ak vrati iny iterator, povodny bude automaticky zruseny. </remarks>
			Iterator<T>& operator++() override;
		private:
			/// <summary> Zoznam, cez ktory iteruje. </summary>
			const ArrayList<T>* arrayList_;
			/// <summary> Aktualna pozicia v zozname. </summary>
			int position_;
		};
	};

	template<typename T>
	inline ArrayList<T>::ArrayList():
		List(),
		array_(new Array<T>(4)),
		size_(0)
	{
	}

	template<typename T>
	inline ArrayList<T>::ArrayList(const ArrayList<T>& other):
		List(),
		array_(new Array<T>(*other.array_)),
		size_(other.size_)
	{
	}

	template<typename T>
	inline ArrayList<T>::~ArrayList()
	{
		delete array_;
		array_ = nullptr;
		size_ = 0;
	}

	template<typename T>
	inline Structure * ArrayList<T>::clone() const
	{
		return new ArrayList<T>(*this);
	}

	template<typename T>
	inline size_t ArrayList<T>::size() const
	{
		return size_;
	}

	template<typename T>
	inline List<T>& ArrayList<T>::operator=(const List<T>& other)
	{
		if (this != &other)
		{
			*this = dynamic_cast<const ArrayList<T>&>(other);
		}
		return *this;
	}

	template<typename T>
	inline ArrayList<T>& ArrayList<T>::operator=(const ArrayList<T>& other)
	{
		if (this != &other) {
			delete array_;
			array_ = new Array<T>(*other.array_);
			size_ = other.size_;
		}
		return *this;
	}

	template<typename T>
	inline T & ArrayList<T>::operator[](const int index)
	{
		DSRoutines::rangeCheckExcept(index, size_, "ArrayList out of index");
		//je to ekvivalentne
		//return array_->operator[](index);
		//return (*array_).operator[](index);
		return (*array_)[index];
	}

	template<typename T>
	inline const T ArrayList<T>::operator[](const int index) const
	{
		DSRoutines::rangeCheckExcept(index, size_, "ArrayList out of index");
		return (*array_)[index];
	}

	template<typename T>
	inline void ArrayList<T>::add(const T & data)
	{
		if (array_->size() == size()) {
			enlarge();
		}
		(*array_)[(int)(size_++)] = data;
	}

	template<typename T>
	inline void ArrayList<T>::insert(const T & data, const int index)
	{
		if (index == static_cast<int>(size_)) {
			add(data);
		}
		else {
			DSRoutines::rangeCheckExcept(index, size_, "index out of range");
			if (array_->size() == size_) {
				enlarge();
			}
			Array<T>::copy(*array_, static_cast<int>(index), *array_, static_cast<int>(index + 1), static_cast<int>(size_ - index));
			(*array_)[index] = data;
			size_++;
		}
	}

	template<typename T>
	inline bool ArrayList<T>::tryRemove(const T & data)
	{
		int index = getIndexOf(data);
		if (index == -1) {
			return false;
		}
		else {
			removeAt(index);
			return true;
		}
	}

	template<typename T>
	inline T ArrayList<T>::removeAt(const int index)
	{
		DSRoutines::rangeCheckExcept(index, size_, "Mimo pole.");

		T temp = (*array_)[index];
		Array<T>::copy(*array_, index + 1, *array_, index, static_cast<int>(size_) - (index + 1));
		size_--;

		return temp;
	}

	template<typename T>
	inline int ArrayList<T>::getIndexOf(const T & data)
	{
		for (int index = 0; index < static_cast<int>(size_); index++)
		{
			if ((*this)[index] == data)
				return index;
		}
		return -1;
	}

	template<typename T>
	inline void ArrayList<T>::clear()
	{
		size_ = 0;
	}

	template<typename T>
	inline Iterator<T>* ArrayList<T>::getBeginIterator() const
	{
		//TODO 03: ArrayList
		throw std::exception("ArrayList<T>::getBeginIterator: Not implemented yet.");
	}

	template<typename T>
	inline Iterator<T>* ArrayList<T>::getEndIterator() const
	{
		//TODO 03: ArrayList
		throw std::exception("ArrayList<T>::getEndIterator: Not implemented yet.");
	}

	template<typename T>
	inline void ArrayList<T>::enlarge()
	{
		Array<T>* tmp = new Array<T>(size_ * 2);
		Array<T>::copy(*array_, 0, *tmp, 0, (int)size_);
		delete array_;
		array_ = tmp;

	}

	template<typename T>
	inline ArrayList<T>::ArrayListIterator::ArrayListIterator(const ArrayList<T>* arrayList, int position) :
		arrayList_(arrayList),
		position_(position)
	{
	}

	template<typename T>
	inline ArrayList<T>::ArrayListIterator::~ArrayListIterator()
	{
		//TODO 03: ArrayList<T>::ArrayListIterator
		// vymazat smernik a vynulovat poziciu
		arrayList_ = nullptr;
		position_ = 0;
	}

	template<typename T>
	inline Iterator<T>& ArrayList<T>::ArrayListIterator::operator=(const Iterator<T>& other)
	{
		//TODO 03: ArrayList
		throw std::exception("ArrayList<T>::ArrayListIterator::operator=: Not implemented yet.");
	}

	template<typename T>
	inline bool ArrayList<T>::ArrayListIterator::operator!=(const Iterator<T>& other)
	{
		//TODO 03: ArrayList
		throw std::exception("ArrayList<T>::ArrayListIterator::operator!=: Not implemented yet.");
	}

	template<typename T>
	inline const T ArrayList<T>::ArrayListIterator::operator*()
	{
		//TODO 03: ArrayList
		throw std::exception("ArrayList<T>::ArrayListIterator::operator*: Not implemented yet.");
	}

	template<typename T>
	inline Iterator<T>& ArrayList<T>::ArrayListIterator::operator++()
	{
		//TODO 03: ArrayList
		throw std::exception("ArrayList<T>::ArrayListIterator::operator++: Not implemented yet.");
	}

}