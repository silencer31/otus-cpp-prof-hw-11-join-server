#include "database_manager.h"

#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>

// ����������� � ���� ������.
bool DatabaseManager::connect_database()
{
    return sql_mngr_ptr->init_database(boost::filesystem::exists(database_path));
}

// �������� ������ � ����.
bool DatabaseManager::insert_data(const std::string& table, const int& id_val, const std::string& name_val)
{
    const std::string name_txt = "'" + name_val + "'";

    const std::string all_values = "(" + std::to_string(id_val) + ","
        + name_txt + ")";

    return sql_mngr_ptr->insert_row(table, all_values);
}

// ������, ���� �� ��� ������ � ����� id � ��������� �������.
bool DatabaseManager::id_present(const std::string& table, const int& id)
{
    return sql_mngr_ptr->get_int_value_presence(table, "id", id);
}

// �������� ������ ����� �� ������� name ��� ��������� id.
void DatabaseManager::get_names_by_ids(const std::string& table, const vector_int& ids, vector_str& names)
{
    std::string name_val;

    for (const int& id_val : ids) {
        if (sql_mngr_ptr->get_corresp_col_value_txt(table, "id", "name", id_val, name_val)) {
            names.push_back(name_val);
        }
    }
}

// �������� ������ �� id, ������� ���� � ����� ��������.
void DatabaseManager::get_equal_ids(vector_int& id_values)
{
    vector_int a_ids;

    // ���� ��� id, ������� ���� � ������� A.
    int a_ids_number = sql_mngr_ptr->get_column_values_int("A", "id", a_ids);

    if (a_ids_number == 0) {
        return;
    }

    id_values.reserve(a_ids_number);

    // ���� ���������� �� id � �������� B.
    for (const int& id_val : a_ids) {
        if (sql_mngr_ptr->get_int_value_presence("B", "id", id_val)) {
            id_values.push_back(id_val);
        }
    }

    id_values.shrink_to_fit();
}

// �������� ������ �� id, ������� ���� � ������ �������, �� ����������� �� ������.
void DatabaseManager::get_unique_ids(const std::string& tab_1, const std::string& tab_2, vector_int& id_values)
{
    vector_int tab_1_ids;

    // ���� ��� id, ������� ���� � ������ �������.
    int tab_1_ids_number = sql_mngr_ptr->get_column_values_int(tab_1, "id", tab_1_ids);

    if (tab_1_ids_number == 0) {
        return;
    }

    id_values.reserve(tab_1_ids_number);

    // ���� ������ �� id, ������� ��� �� ������ �������.
    for (const int& id_val : tab_1_ids) {
        if (sql_mngr_ptr->get_int_value_presence(tab_2, "id", id_val)) {
            continue;
        }

        id_values.push_back(id_val);
    }

    id_values.shrink_to_fit();
}

// ��������� ������ � ���� ������.
RequestResult DatabaseManager::commit_db_request(const DatabaseRequest & db_request)
{
    RequestResult req_result;
    std::string table_name;

    switch (db_request.request_type) {
    case RequestType::UNKNOWN:
        return RequestResult( ResultType::ERR, "Unknown request type");
        break;
    case RequestType::INSERT:
        table_name = (db_request.data_table == DataTable::A ? "A" : "B");
        if (id_present(table_name, db_request.id)) {
            return RequestResult(ResultType::ERR, boost::str(boost::format("duplicate %1%") % db_request.id));
        }
        if (insert_data(table_name, db_request.id, db_request.name)) {
            return RequestResult(ResultType::OK);
        }
        return RequestResult(ResultType::ERR, "Unable to insert data");
        break;
    case RequestType::TRUNCATE:
        table_name = (db_request.data_table == DataTable::A ? "A" : "B");
        if (sql_mngr_ptr->truncate_table(table_name)) {
            return RequestResult(ResultType::OK);
        }
        return RequestResult(ResultType::ERR, "Unable to truncate table");
        break;
    case RequestType::INTERSECTION:
        return commit_intersection();
        break;
    case RequestType::DIFFERENCE:
        return commit_difference();
        break;
    default:
        return RequestResult(ResultType::ERR, "Unknown request type");
        break;
    };

    return req_result;
}

// ��������� ����� ����� � ���������� id.
RequestResult DatabaseManager::commit_intersection()
{
    vector_int equal_ids;
    get_equal_ids(equal_ids);

    if (equal_ids.empty()) {
        return RequestResult(ResultType::OK);
    }

    RequestResult req_result;

    vector_str a_names;
    get_names_by_ids("A", equal_ids, a_names);
    
    vector_str b_names;
    get_names_by_ids("B", equal_ids, b_names);

    for ( int i = 0; i < equal_ids.size(); ++ i) {
        req_result.res_strings.push(boost::str(boost::format("%1%,%2%,%3%") % equal_ids.at(i) % a_names.at(i) % b_names.at(i)));
    }

    return req_result;
}

// ��������� ����� ����� � ����������� id.
RequestResult DatabaseManager::commit_difference()
{
    vector_int a_unique_ids;
    get_unique_ids("A", "B", a_unique_ids);

    vector_int b_unique_ids;
    get_unique_ids("b", "A", b_unique_ids);

    if (a_unique_ids.empty() && b_unique_ids.empty()) {
        return RequestResult(ResultType::OK);
    }

    RequestResult req_result;

    vector_str a_names;
    get_names_by_ids("A", a_unique_ids, a_names);

    vector_str b_names;
    get_names_by_ids("B", b_unique_ids, b_names);

    for (int i = 0; i < a_unique_ids.size(); ++i) {
        req_result.res_strings.push(boost::str(boost::format("%1%,%2%,") % a_unique_ids.at(i) % a_names.at(i) ));
    }

    for (int i = 0; i < b_unique_ids.size(); ++i) {
        req_result.res_strings.push(boost::str(boost::format("%1%,,%2%") % b_unique_ids.at(i) % b_names.at(i)));
    }

    return req_result;
}

// ����� � ��������� ������ ������� � ������� ��������.
void DatabaseManager::new_request_notify()
{
    // ��������� ��������� ����� � ��������� ������ ������� � ����.
    cond_var.notify_one();
}

// �����, ���������� � ��������� ������.
void DatabaseManager::worker_thread()
{
    thread_active = true;

    while (!done) {
        // ����������� ������� �������� ��������� ������ �������.
        std::unique_lock<std::mutex> req_wait_lock(wait_mutex);

        // ��������� ������� � �������, ���� � ������� �������� ����� ������.
        cond_var.wait(req_wait_lock, [this]()->bool { return !req_coll_ptr->empty() || done; });

        if (done) {
            break;
        }

        // ������� ����� ��������.
        if (req_coll_ptr->empty()) {
            req_wait_lock.unlock();
            continue;
        }

        // ���� ������ ������ � ��������� � ���������.
        const session_request id_req = std::move(req_coll_ptr->front());

        // ������� �� ������� �������� � ��������� �������.
        req_coll_ptr->pop();

        // ��������� ������ � ���� ������.
        const RequestResult result = commit_db_request(id_req.second);

        // ��������� �������.
        req_wait_lock.unlock();

        // ���������� ��������� ���������� � ��������� � ������������.
        res_coll_ptr->add_result(id_req.first, result);

        // ��������� ��������������� ������ � ���������� �������.
        notifier_ptr->notify(id_req.first);
    }

    thread_active = false;

    std::cout << "^_^_^" << std::endl;
}

