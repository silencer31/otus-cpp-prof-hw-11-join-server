#pragma once

#include <memory>
#include <list>

/**
* @brief Класс - интерфейс для классов - наблюдателей.
*/
class Observer {
public:
	virtual ~Observer() = default;

	/**
	* Реакция на изменения в наблюдаемом объекте.
	*/
	virtual void update() = 0;

protected:
	Observer() = default;
};

using shared_observer = std::shared_ptr<Observer>;
using observers_list = std::list<std::shared_ptr<Observer>>;

/**
* @brief Класс - интерфейс для классов, за изменениями в которых возможно наблюдение.
*/
class Observable {
public:

	/**
	* Добавление обозревателя в коллекцию.
	* @param observer умный указатель на добавляемый обозреватель.
	*/
	virtual void attach(const shared_observer& observer) {
		observers.push_back(observer);
	}

	/**
	* Удаление обозревателя из коллекции.
	* @param observer умный указатель на удаляемый обозреватель.
	*/
	virtual void detach(const shared_observer& observer) {
		observers.remove_if([observer](shared_observer so) { return so == observer; });
	}

	/**
	* Оповещение всех обозревателей об изменениях в модели.
	*/
	virtual void notify() {
		for (observers_list::const_iterator obs_sptr = observers.cbegin(); obs_sptr != observers.cend(); ++obs_sptr) {
			(*obs_sptr)->update();
		}
	}

protected:
	Observable() = default;

private:
	observers_list observers; // Коллекция обозревателей.
};