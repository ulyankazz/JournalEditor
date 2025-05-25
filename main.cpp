#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <set>
#include <thread>
#include <chrono>

using namespace std;

// Множество для хранения уникальных имён журналистов
set<string> registeredJournalists;

// Множество для хранения всех вошедших пользователей
set<string> loggedUsers;

class Article {
private:
    int id;
    string title;
    string text;
    string status;
    string author;
    string assignedJournalist;
    string comments; // Для хранения правок/комментариев
    string typesettingText; // Для хранения текста верстки
public:
    Article(int i, string t, string txt, string a) : id(i), title(t), text(txt), author(a), status("Draft"), 
                                                    assignedJournalist(""), comments(""), typesettingText("") {}
    void setId(int i) { id = i; }
    void setStatus(string s) { status = s; }
    void setAssignedJournalist(string j) { assignedJournalist = j; }
    void setComments(string c) { comments = c; }
    void setTypesettingText(string t) { typesettingText = t; }
    int getId() const { return id; }
    string getStatus() const { return status; }
    string getTitle() const { return title; }
    string getText() const { return text; }
    string getAuthor() const { return author; }
    string getAssignedJournalist() const { return assignedJournalist; }
    string getComments() const { return comments; }
    string getTypesettingText() const { return typesettingText; }
    void display() const {
        cout << "Статья ID: " << id << ", " << title << ", Автор: " << author << ", Статус: " << status;
        if (!assignedJournalist.empty()) cout << ", Назначена: " << assignedJournalist;
        cout << endl;
    }
    void saveToFile(ofstream& outFile) const {
        outFile << id << "|" << title << "|" << text << "|" << status << "|" << author << "|" << assignedJournalist 
                << "|" << comments << "|" << typesettingText << "\n";
    }
    static Article loadFromFile(int i, const string& line) {
        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|", pos1 + 1);
        size_t pos3 = line.find("|", pos2 + 1);
        size_t pos4 = line.find("|", pos3 + 1);
        size_t pos5 = line.find("|", pos4 + 1);
        size_t pos6 = line.find("|", pos5 + 1);
        size_t pos7 = line.find("|", pos6 + 1);
        string title = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string text = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string status = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string author = line.substr(pos4 + 1, pos5 - pos4 - 1);
        string assignedJournalist = line.substr(pos5 + 1, pos6 - pos5 - 1);
        string comments = line.substr(pos6 + 1, pos7 - pos6 - 1);
        string typesettingText = line.substr(pos7 + 1);
        Article article(i, title, text, author);
        article.setStatus(status);
        article.setAssignedJournalist(assignedJournalist);
        article.setComments(comments);
        article.setTypesettingText(typesettingText);
        return article;
    }
};

class User {
protected:
    string role;
    string password;
public:
    User(string r, string p) : role(r), password(p) {}
    string getRole() { return role; }
    string getPassword() { return password; }
    virtual void performAction(vector<Article>& articles) = 0;
    virtual void showPage(vector<Article>& articles) {}
    virtual ~User() = default;
};

class Reader : public User {
public:
    Reader(string p) : User("Reader", p) {
        loggedUsers.insert(p);
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "Reader|" << p << "\n";
            outFile.close();
        }
    }
    void performAction(vector<Article>& articles) override {
        cout << password << " читает статью или оставляет отзыв." << endl;
    }
    void showPage(vector<Article>& articles) override {
        cout << "Добро пожаловать, " << password << "! Вы вошли как Читатель.\n";
        cout << "Ваши действия:\n";
        cout << "- 1. Просмотреть опубликованные статьи\n";
        cout << "- 2. Подписаться на уведомления\n";
        cout << "- 3. Оставить отзыв\n";
        cout << "- 4. Выйти из аккаунта\n";
        if (!articles.empty()) {
            cout << "Опубликованные статьи:\n";
            for (const auto& article : articles) {
                if (article.getStatus() == "Published") article.display();
            }
        }
    }
};

class ChiefEditor : public User {
public:
    ChiefEditor(string p) : User("ChiefEditor", p) {
        loggedUsers.insert(p);
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "ChiefEditor|" << p << "\n";
            outFile.close();
        }
    }
    void performAction(vector<Article>& articles) override {
        cout << password << " формирует план выпуска или проверяет верстку." << endl;
    }
    void showPage(vector<Article>& articles) override {
        cout << "Добро пожаловать, " << password << "! Вы вошли как Главный редактор.\n";
        cout << "Ваши действия:\n";
        cout << "- 1. Сформировать план выпуска и передать редактору\n";
        cout << "- 2. Утвердить или отправить верстку на доработку\n";
        cout << "- 3. Выйти из аккаунта\n";
    }
};

class Editor : public User {
public:
    Editor(string p) : User("Editor", p) {
        loggedUsers.insert(p);
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "Editor|" << p << "\n";
            outFile.close();
        }
    }
    void performAction(vector<Article>& articles) override {
        cout << password << " выполняет редакторские действия." << endl;
    }
    void showPage(vector<Article>& articles) override {
        cout << "Добро пожаловать, " << password << "! Вы вошли как Редактор.\n";
        cout << "Ваши действия:\n";
        cout << "- 1. Назначить статью журналисту\n";
        cout << "- 2. Внести правки и отправить журналисту\n";
        cout << "- 3. Отправить материал верстальщику\n";
        cout << "- 4. Принять статью\n";
        cout << "- 5. Выйти из аккаунта\n";
    }
};

class Journalist : public User {
public:
    Journalist(string p) : User("Journalist", p) {
        loggedUsers.insert(p);
        registeredJournalists.insert(p);
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "Journalist|" << p << "\n";
            outFile.close();
        }
    }
    void performAction(vector<Article>& articles) override {
        cout << password << " работает над статьей." << endl;
    }
    void showPage(vector<Article>& articles) override {
        cout << "Добро пожаловать, " << password << "! Вы вошли как Журналист.\n";
        cout << "Ваши действия:\n";
        cout << "- 1. Выбрать статью для работы\n";
        cout << "- 2. Исправить правки и отправить редактору\n";
        cout << "- 3. Выйти из аккаунта\n";
        cout << "Назначенные статьи:\n";
        vector<int> assignedArticles;
        for (size_t i = 0; i < articles.size(); ++i) {
            if (articles[i].getAssignedJournalist() == password && 
                (articles[i].getStatus() == "InProgress" || articles[i].getStatus() == "NeedsRevision")) {
                cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                     << ", Комментарии: " << articles[i].getComments() << ")\n";
                assignedArticles.push_back(i);
            }
        }
        if (assignedArticles.empty()) cout << "Нет назначенных статей.\n";
    }
};

class Typesetter : public User {
public:
    Typesetter(string p) : User("Typesetter", p) {
        loggedUsers.insert(p);
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "Typesetter|" << p << "\n";
            outFile.close();
        }
    }
    void performAction(vector<Article>& articles) override {
        cout << password << " выполняет верстку." << endl;
    }
    void showPage(vector<Article>& articles) override {
        cout << "Добро пожаловать, " << password << "! Вы вошли как Верстальщик.\n";
        cout << "Ваши действия:\n";
        cout << "- 1. Сделать верстку\n";
        cout << "- 2. Отправить готовый макет администратору\n";
        cout << "- 3. Выйти из аккаунта\n";
        cout << "Материалы для верстки:\n";
        vector<int> typesettingArticles;
        for (size_t i = 0; i < articles.size(); ++i) {
            if (articles[i].getStatus() == "ReadyForTypesetting" || articles[i].getStatus() == "NeedsRevision") {
                cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() << ")\n";
                typesettingArticles.push_back(i);
            }
        }
        if (typesettingArticles.empty()) cout << "Нет материалов для верстки.\n";
    }
};

class Administrator : public User {
public:
    Administrator(string p) : User("Administrator", p) {
        loggedUsers.insert(p);
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << "Administrator|" << p << "\n";
            outFile.close();
        }
    }
    void performAction(vector<Article>& articles) override {
        cout << password << " публикует выпуск и рассылает уведомления." << endl;
    }
    void showPage(vector<Article>& articles) override {
        cout << "Добро пожаловать, " << password << "! Вы вошли как Администратор.\n";
        cout << "Ваши действия:\n";
        cout << "- 1. Опубликовать выпуск\n";
        cout << "- 2. Разослать уведомления читателям\n";
        cout << "- 3. Выйти из аккаунта\n";
    }
};

// Функция для очистки ввода
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Функция для очистки экрана
void clearScreen() {
    for (int i = 0; i < 50; ++i) {
        cout << "\n";
    }
}

// Функция для паузы перед очисткой
void pauseBeforeClear() {
    cout << "Нажмите Enter, чтобы продолжить...\n";
    cin.get();
}

// Функция для загрузки статей из файла с уникальными ID
vector<Article> loadArticles() {
    vector<Article> articles;
    ifstream inFile("articles.txt");
    int id = 1;
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                articles.push_back(Article::loadFromFile(id, line));
                id++;
            }
        }
        inFile.close();
    }
    cout << "Загружено статей: " << articles.size() << endl;
    for (const auto& article : articles) {
        cout << "Загружено: ID " << article.getId() << ", " << article.getTitle() << " (Статус: " << article.getStatus() 
             << ", Назначена: " << article.getAssignedJournalist() << ")\n";
    }
    return articles;
}

// Функция для загрузки пользователей из файла
void loadUsers() {
    ifstream inFile("users.txt");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                size_t pos = line.find("|");
                string role = line.substr(0, pos);
                string password = line.substr(pos + 1);
                if (role == "Journalist") registeredJournalists.insert(password);
                loggedUsers.insert(password);
            }
        }
        inFile.close();
    }
    cout << "Загружено пользователей: " << loggedUsers.size() << endl;
}

// Функция для сохранения статей в файл
void saveArticles(const vector<Article>& articles) {
    ofstream outFile("articles.txt");
    if (outFile.is_open()) {
        for (const auto& article : articles) {
            article.saveToFile(outFile);
        }
        outFile.close();
    }
    cout << "Сохранено статей: " << articles.size() << endl;
    for (const auto& article : articles) {
        cout << "Сохранено: ID " << article.getId() << ", " << article.getTitle() << " (Статус: " << article.getStatus() 
             << ", Назначена: " << article.getAssignedJournalist() << ")\n";
    }
}

// Функция для удаления файлов
void clearFiles() {
    remove("articles.txt");
    remove("users.txt");
    cout << "Файлы очищены.\n";
}

// Универсальное меню для всех ролей
void userMenu(User* user, vector<Article>& articles) {
    while (true) {
        clearScreen();
        user->showPage(articles);
        cout << "\nВыберите действие (введите номер или 'выход' для завершения программы): ";
        string action;
        getline(cin, action);

        if (action == "выход") {
            saveArticles(articles);
            clearFiles();
            break;
        }

        if (user->getRole() == "ChiefEditor" && action == "1") {
            string title;
            cout << "Введите название статьи для плана выпуска: ";
            getline(cin, title);
            if (!title.empty()) {
                articles.emplace_back(articles.size() + 1, title, "", user->getPassword());
                articles.back().setStatus("Assigned");
                cout << "План выпуска сформирован и передан редактору.\n";
                user->performAction(articles);
                saveArticles(articles);
            }
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "ChiefEditor" && action == "2") {
            bool found = false;
            for (auto& article : articles) {
                if (article.getStatus() == "Typeset") {
                    found = true;
                    cout << "Верстка статьи ID: " << article.getId() << ", " << article.getTitle() 
                         << ". Текст верстки: " << article.getTypesettingText() 
                         << ". Утвердить? (да/нет): ";
                    string approve;
                    getline(cin, approve);
                    if (approve == "да") {
                        article.setStatus("ReadyForPublication");
                        cout << "Верстка утверждена.\n";
                    } else {
                        article.setStatus("NeedsRevision");
                        cout << "Верстка отправлена на доработку.\n";
                    }
                    user->performAction(articles);
                    saveArticles(articles);
                    break;
                }
            }
            if (!found) cout << "Нет версток для утверждения.\n";
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "ChiefEditor" && action == "3") {
            delete user;
            return;
        }
        else if (user->getRole() == "Editor" && action == "1") {
            cout << "Доступные статьи для назначения:\n";
            vector<int> availableArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getStatus() == "Assigned") {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                         << ", Назначена: " << articles[i].getAssignedJournalist() << ")\n";
                    availableArticles.push_back(i);
                }
            }
            if (availableArticles.empty()) {
                cout << "Нет назначенных статей.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи для назначения: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < availableArticles.size(); ++i) {
                if (articles[availableArticles[i]].getId() == choice) {
                    articleIndex = availableArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }

            cout << "Зарегистрированные журналисты:\n";
            vector<string> journalistList(registeredJournalists.begin(), registeredJournalists.end());
            if (journalistList.empty()) {
                cout << "Нет зарегистрированных журналистов. Журналисты должны сначала войти в систему.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            for (size_t i = 0; i < journalistList.size(); ++i) {
                cout << i + 1 << ". " << journalistList[i] << "\n";
            }
            cout << "Выберите номер журналиста: ";
            getline(cin, choiceStr);
            int journalistChoice;
            try {
                journalistChoice = stoi(choiceStr) - 1;
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            if (journalistChoice < 0 || journalistChoice >= static_cast<int>(journalistList.size())) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            string selectedJournalist = journalistList[journalistChoice];
            articles[articleIndex].setAssignedJournalist(selectedJournalist);
            articles[articleIndex].setStatus("InProgress");
            cout << "Статья ID " << articles[articleIndex].getId() << " назначена журналисту " << selectedJournalist << ".\n";
            saveArticles(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Editor" && action == "2") {
            cout << "Доступные черновики:\n";
            vector<int> draftArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getStatus() == "Draft") {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                         << ", Назначена: " << articles[i].getAssignedJournalist() << ")\n";
                    draftArticles.push_back(i);
                }
            }
            if (draftArticles.empty()) {
                cout << "Нет черновиков для правки.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи для правки: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < draftArticles.size(); ++i) {
                if (articles[draftArticles[i]].getId() == choice) {
                    articleIndex = draftArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            string revisions;
            cout << "Введите текст правок для статьи ID " << articles[articleIndex].getId() << ", " << articles[articleIndex].getTitle() << ": ";
            getline(cin, revisions);
            if (!revisions.empty()) {
                articles[articleIndex].setComments(revisions); // Сохраняем правки как комментарий
                articles[articleIndex].setStatus("NeedsRevision");
                cout << "Правки внесены в статью ID " << articles[articleIndex].getId() << " и отправлены журналисту.\n";
                user->performAction(articles);
                saveArticles(articles);
            }
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Editor" && action == "3") {
            cout << "Доступные статьи для отправки верстальщику:\n";
            vector<int> acceptedArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getStatus() == "Accepted") {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                         << ", Назначена: " << articles[i].getAssignedJournalist() << ")\n";
                    acceptedArticles.push_back(i);
                }
            }
            if (acceptedArticles.empty()) {
                cout << "Нет принятых статей для отправки верстальщику.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи для отправки: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < acceptedArticles.size(); ++i) {
                if (articles[acceptedArticles[i]].getId() == choice) {
                    articleIndex = acceptedArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            articles[articleIndex].setStatus("ReadyForTypesetting");
            cout << "Материал ID " << articles[articleIndex].getId() << " отправлен верстальщику.\n";
            user->performAction(articles);
            saveArticles(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Editor" && action == "4") {
            cout << "Доступные статьи для принятия:\n";
            vector<int> revisableArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getStatus() == "Revised") {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                         << ", Назначена: " << articles[i].getAssignedJournalist() << ")\n";
                    revisableArticles.push_back(i);
                }
            }
            if (revisableArticles.empty()) {
                cout << "Нет исправленных статей для принятия.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи для принятия: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < revisableArticles.size(); ++i) {
                if (articles[revisableArticles[i]].getId() == choice) {
                    articleIndex = revisableArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            articles[articleIndex].setStatus("Accepted");
            cout << "Статья ID " << articles[articleIndex].getId() << " принята.\n";
            user->performAction(articles);
            saveArticles(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Editor" && action == "5") {
            delete user;
            return;
        }
        else if (user->getRole() == "Journalist" && action == "1") {
            cout << "Выберите статью для работы:\n";
            vector<int> assignedArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getAssignedJournalist() == user->getPassword() && 
                    articles[i].getStatus() == "InProgress") {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                         << ", Комментарии: " << articles[i].getComments() << ")\n";
                    assignedArticles.push_back(i);
                }
            }
            if (assignedArticles.empty()) {
                cout << "Нет назначенных статей для работы.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < assignedArticles.size(); ++i) {
                if (articles[assignedArticles[i]].getId() == choice) {
                    articleIndex = assignedArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            string text;
            cout << "Введите текст статьи ID " << articles[articleIndex].getId() << ", " << articles[articleIndex].getTitle() << ": ";
            getline(cin, text);
            if (!text.empty()) {
                articles[articleIndex].setStatus("Draft");
                cout << "Черновик статьи ID " << articles[articleIndex].getId() << " отправлен редактору.\n";
            }
            user->performAction(articles);
            saveArticles(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Journalist" && action == "2") {
            cout << "Выберите статью для исправления:\n";
            vector<int> revisionArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getStatus() == "NeedsRevision" && articles[i].getAssignedJournalist() == user->getPassword()) {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() 
                         << ", Комментарии: " << articles[i].getComments() << ")\n";
                    revisionArticles.push_back(i);
                }
            }
            if (revisionArticles.empty()) {
                cout << "Нет статей для исправления.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < revisionArticles.size(); ++i) {
                if (articles[revisionArticles[i]].getId() == choice) {
                    articleIndex = revisionArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            string text;
            cout << "Введите исправленный текст статьи ID " << articles[articleIndex].getId() << ", " << articles[articleIndex].getTitle() << ": ";
            getline(cin, text);
            if (!text.empty()) {
                articles[articleIndex].setStatus("Revised");
                cout << "Исправления в статье ID " << articles[articleIndex].getId() << " отправлены редактору.\n";
            }
            user->performAction(articles);
            saveArticles(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Journalist" && action == "3") {
            delete user;
            return;
        }
        else if (user->getRole() == "Typesetter" && action == "1") {
            cout << "Выберите материал для верстки:\n";
            vector<int> typesettingArticles;
            for (size_t i = 0; i < articles.size(); ++i) {
                if (articles[i].getStatus() == "ReadyForTypesetting" || articles[i].getStatus() == "NeedsRevision") {
                    cout << articles[i].getId() << ". " << articles[i].getTitle() << " (Статус: " << articles[i].getStatus() << ")\n";
                    typesettingArticles.push_back(i);
                }
            }
            if (typesettingArticles.empty()) {
                cout << "Нет материалов для верстки.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            cout << "Выберите ID статьи: ";
            string choiceStr;
            getline(cin, choiceStr);
            int choice;
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Неверный выбор.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            int articleIndex = -1;
            for (size_t i = 0; i < typesettingArticles.size(); ++i) {
                if (articles[typesettingArticles[i]].getId() == choice) {
                    articleIndex = typesettingArticles[i];
                    break;
                }
            }
            if (articleIndex == -1) {
                cout << "Статья с ID " << choice << " не найдена.\n";
                pauseBeforeClear();
                clearScreen();
                continue;
            }
            string typesettingText;
            cout << "Введите текст верстки для статьи ID " << articles[articleIndex].getId() << ", " << articles[articleIndex].getTitle() << ": ";
            getline(cin, typesettingText);
            if (!typesettingText.empty()) {
                articles[articleIndex].setTypesettingText(typesettingText);
                articles[articleIndex].setStatus("Typeset");
                cout << "Верстка статьи ID " << articles[articleIndex].getId() << " отправлена главному редактору.\n";
                user->performAction(articles);
                saveArticles(articles);
            }
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Typesetter" && action == "2") {
            bool found = false;
            for (auto& article : articles) {
                if (article.getStatus() == "ReadyForPublication") {
                    found = true;
                    article.setStatus("ReadyForPublication");
                    cout << "Готовый макет ID " << article.getId() << " отправлен администратору.\n";
                    user->performAction(articles);
                    saveArticles(articles);
                    break;
                }
            }
            if (!found) cout << "Нет утверждённых версток для отправки.\n";
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Typesetter" && action == "3") {
            delete user;
            return;
        }
        else if (user->getRole() == "Administrator" && action == "1") {
            bool found = false;
            for (auto& article : articles) {
                if (article.getStatus() == "ReadyForPublication") {
                    found = true;
                    article.setStatus("Published");
                    cout << "Выпуск с статьей ID " << article.getId() << " опубликован.\n";
                    user->performAction(articles);
                    saveArticles(articles);
                    break;
                }
            }
            if (!found) cout << "Нет готовых макетов для публикации.\n";
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Administrator" && action == "2") {
            bool found = false;
            for (const auto& article : articles) {
                if (article.getStatus() == "Published") {
                    found = true;
                    cout << "Уведомления о публикации статьи ID " << article.getId() << " разосланы читателям.\n";
                    user->performAction(articles);
                    break;
                }
            }
            if (!found) cout << "Нет опубликованных статей для уведомлений.\n";
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Administrator" && action == "3") {
            delete user;
            return;
        }
        else if (user->getRole() == "Reader" && action == "1") {
            cout << "Просмотр опубликованных статей:\n";
            for (const auto& article : articles) {
                if (article.getStatus() == "Published") article.display();
            }
            user->performAction(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Reader" && action == "2") {
            cout << "Подписка на уведомления оформлена.\n";
            user->performAction(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Reader" && action == "3") {
            string review;
            cout << "Оставьте отзыв: ";
            getline(cin, review);
            cout << "Отзыв от " << user->getPassword() << ": " << review << " отправлен.\n";
            user->performAction(articles);
            pauseBeforeClear();
            clearScreen();
        }
        else if (user->getRole() == "Reader" && action == "4") {
            delete user;
            return;
        }
        else {
            cout << "Неверное действие! Попробуйте снова.\n";
            pauseBeforeClear();
            clearScreen();
        }
    }
}

int main() {
    loadUsers();
    vector<Article> articles = loadArticles();

    while (true) {
        string login, password;
        cout << "Введите логин (роль): ";
        getline(cin, login);
        cout << "Введите пароль (имя): ";
        getline(cin, password);

        User* currentUser = nullptr;
        if (login == "Reader") currentUser = new Reader(password);
        else if (login == "ChiefEditor") currentUser = new ChiefEditor(password);
        else if (login == "Editor") currentUser = new Editor(password);
        else if (login == "Journalist") currentUser = new Journalist(password);
        else if (login == "Typesetter") currentUser = new Typesetter(password);
        else if (login == "Administrator") currentUser = new Administrator(password);

        if (currentUser == nullptr) {
            cout << "Неверная роль! Доступные роли: Reader, ChiefEditor, Editor, Journalist, Typesetter, Administrator.\n";
            pauseBeforeClear();
            clearScreen();
            continue;
        }

        userMenu(currentUser, articles);

        delete currentUser;
    }

    return 0;
}