#pragma once

#include <memory>
#include <list>

/**
* @brief ����� - ��������� ��� ������� - ������������.
*/
class Observer {
public:
	virtual ~Observer() = default;

	/**
	* ������� �� ��������� � ����������� �������.
	*/
	virtual void update() = 0;

protected:
	Observer() = default;
};

using shared_observer = std::shared_ptr<Observer>;
using observers_list = std::list<std::shared_ptr<Observer>>;

/**
* @brief ����� - ��������� ��� �������, �� ����������� � ������� �������� ����������.
*/
class Observable {
public:

	/**
	* ���������� ������������ � ���������.
	* @param observer ����� ��������� �� ����������� ������������.
	*/
	virtual void attach(const shared_observer& observer) {
		observers.push_back(observer);
	}

	/**
	* �������� ������������ �� ���������.
	* @param observer ����� ��������� �� ��������� ������������.
	*/
	virtual void detach(const shared_observer& observer) {
		observers.remove_if([observer](shared_observer so) { return so == observer; });
	}

	/**
	* ���������� ���� ������������� �� ���������� � ������.
	*/
	virtual void notify() {
		for (observers_list::const_iterator obs_sptr = observers.cbegin(); obs_sptr != observers.cend(); ++obs_sptr) {
			(*obs_sptr)->update();
		}
	}

protected:
	Observable() = default;

private:
	observers_list observers; // ��������� �������������.
};