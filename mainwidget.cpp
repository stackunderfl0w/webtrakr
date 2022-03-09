#include <iostream>
#include <regex>
#include "web-helper.h"
#include <QtWidgets/QtWidgets>
#include <QDesktopServices>
#include <fstream>
#include <deque>
#include <atomic>
#include <thread>
#include "qrightclickbutton.h"
#include "mainwidget.h"

#include <json.hpp>

#define CRASH     *((char*)-1) = 'x';

struct chapter{
    char* name=nullptr;
    char* address=nullptr;
    bool read=false;
};
struct manga{
    char* title=nullptr;
    char* address=nullptr;
    char* type=nullptr;
    bool loaded=false;
    std::vector<chapter*> chapters;
};
static char *strrstr(const char *haystack, const char *needle){
    if (*needle == '\0')
        return (char *) haystack;

    char *result = NULL;
    for (;;) {
        char *p = const_cast<char *>(strstr(haystack, needle));
        if (p == NULL)
            break;
        result = p;
        haystack = p + 1;
    }
    return result;
}
int chapter_num_from_str(const std::string& str){
    size_t last_index = str.find_last_not_of("0123456789");
    std::string result = str.substr(last_index + 1);
    std::cout<<std::stoi(result)<<'\n';
    return stoi(result);
}

std::deque<manga*> loader_queue;
std::atomic_bool deque_lock;
std::atomic_bool running;

void MainWidget::manga_loader(){
    while (running){
        loader_queue.empty()? loadingicon->hide():loadingicon->show();
        if(not deque_lock && !loader_queue.empty()){
            std::vector<std::string> links;
            deque_lock = true;
            manga* current_target=loader_queue.front();
            loader_queue.pop_front();
            deque_lock = false;

            if(strstr(current_target->address,"wuxia")){
                links = get_chapter_list_wuxia(current_target->address);
            }
            else{
                links = get_chapter_list(current_target->address);
            }
            for (auto x:links) {
                std::cout<<x<<std::endl;
            }
            //std::reverse(links.begin(), links.end());
			std::string root;
			for(int i = links.size() - 1; i >= 0; i--){
				if(strstr(links[i].c_str(),"chapter")&&!strstr(links[i].c_str(),"assets")){
					root=links[i].substr(0,links[i].rfind("chapter"));
					break;
				}
			}
			assert(!root.empty());
            //std::string root = links[links.size()-2].substr(0,links[links.size()-2].rfind("chapter"));
            //std::cout<<links[links.size()-1]<<std::endl;
            std::cout<<root<<std::endl;
            std::vector<std::string> processed_links;

            for (const auto& x:links) {
                std::string str=x;
                if(str.back()=='/'){
                    str.pop_back();
                }
                if(str.back()=='-'){
                    continue;
                }
                std::string substr=std::string(strrstr(str.c_str(), "chapter"));
                //check if its the same series and also not some random link
                //std::cout<<root.c_str()<<std::endl;
                if(substr.length()<40&& strstr(str.c_str(),root.c_str())){
                    if (processed_links.empty()){
                        processed_links.emplace_back(str);
                    }
                    else if (str!=processed_links.back()){
                        processed_links.emplace_back(str);
                    }
                }
            }
            if (strstr(root.c_str(),"mangakakalot")){
                //ignore the latest chapter reapearing in the top bar.for this site i aparently need to do that twice due to the way i handle it the first time
                for (int i = 1; i < processed_links.size(); ++i) {
                    if(!strcmp(processed_links[i].c_str(), processed_links[0].c_str())){
                        processed_links.erase(processed_links.begin());
                        std::cout<<"erased"<<std::endl;
                        break;
                    }
                }
                //ignore the 2 annoying potential future releases
                processed_links.erase(processed_links.begin());
                processed_links.erase(processed_links.begin());
            }
            //make sure we dont duplicate everything, but only do the checks if necesary
            if (current_target->chapters.empty()){
                for (auto str:processed_links) {
                    std::cout<<str<<std::endl;
                    chapter* temp = new chapter();
                    temp->name=strdup(strrstr(str.c_str(),"chapter"));
                    temp->address=strdup(str.c_str());
                    current_target->chapters.emplace_back(temp);
                }
            }
            else{
                //auto start = std::chrono::high_resolution_clock::now();
                int index=0;
                for (const auto& str:processed_links) {
                    char* name=strrstr(str.c_str(),"chapter");
                    bool found=false;
                    //maybe find a better way to do this bullshit. //eh it checks over 800 in 1ms its fine
                    for (auto &i: current_target->chapters){
                        if(!strcmp(i->name, name)){
                            //printf("Already found\n");
                            found=true;
                            break;
                        }
                    }
                    if (!found){
                        chapter* temp = new chapter();
                        temp->name=strdup(strrstr(str.c_str(),"chapter"));
                        temp->address=strdup(str.c_str());
                        //current_target->chapters.emplace_back(temp);
                        std::cout<<"first chapter "<<current_target->chapters[0]->name<<std::endl;
                        std::cout<<chapter_num_from_str(current_target->chapters[0]->name)<<std::endl;
                        if (chapter_num_from_str(current_target->chapters[0]->name) > 1){
                            current_target->chapters.insert(current_target->chapters.begin()+index, temp);
                            std::cout<<"insert at "<<index<<std::endl;

                        }
                        else{
                            current_target->chapters.emplace_back(temp);
                            std::cout<<"emplaced back at "<<current_target->chapters.size()<<std::endl;
                        }
                        //current_target->chapters.insert(current_target->chapters.begin()+index, temp);
                        //std::cout<<"insert at "<<index<<std::endl;
                        index++;
                    }
                }
                //auto end = std::chrono::high_resolution_clock::now();
                //std::cout<<"Time to check for dupes(ms)"<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<std::endl;
            }
            //links.clear();
            //processed_links.clear();
            current_target->loaded=true;
            int i=0;
            std::cout<<current_target->chapters.size()<<std::endl;
            if(title->text().toStdString()== current_target->title){
                emit(reload_chapter_buttons_signal());
            }
            //std::reverse(current_target->chapters.begin(),current_target->chapters.end());
            //CRASH
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::map<std::string, manga*> mangas;
std::thread *curler;
//std::thread *curler2;

// Constructor for main window
MainWidget::MainWidget(QWidget *parent) :
        QWidget(parent)
{
    connect(this, SIGNAL(reload_chapter_buttons_signal()), this, SLOT(reload_chapter_buttons()));
    running=true;
    deque_lock=true;
    mainLayout = new QGridLayout;
    titleLayout= new QGridLayout;
    new_manga = new QGridLayout;
    //used later

    title = new QLabel;
    backButton= new QPushButton;
    addButton= new QPushButton;
    reloadButton= new QPushButton;
    scrollDownButton= new QPushButton;
    loadingicon = new QLabel;
    //loadingmovie = new QMovie("../ajax-loader.gif");
    loadingmovie = new QMovie(":/ajax-loader.gif");
    loadingicon->setMovie(loadingmovie);
    new_manga_carrier = new QWidget;

    title->setText("Webtrakr");
    title->setAlignment(Qt::AlignHCenter);
    std::cout<<title->font().pointSize()<<"\n";
    title->setFont(QFont("Arial", 12));

    backButton->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    reloadButton->setIcon(QIcon(":/reload.png"));
    scrollDownButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    //addButton->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    addButton->setText("+");
    addButton->setFont(QFont("Arial", 16));
    addButton->setFlat(true);
    addButton->setMaximumSize(25,25);
    connect(backButton, SIGNAL(released()), this, SLOT(back_button_clicked()));
    connect(addButton, SIGNAL(released()), this, SLOT(add_button_clicked()));
    connect(reloadButton, SIGNAL(released()), this, SLOT(reload_manga()));
    connect(scrollDownButton, SIGNAL(released()), this, SLOT(scrollDown()));

    titleLayout->addWidget(backButton,0,0,1,1);
    titleLayout->addWidget(loadingicon,0,1,1,1);
    titleLayout->addWidget(title,0,2,1,8);
    titleLayout->addWidget(addButton,0,11,1,1);
    titleLayout->addWidget(reloadButton,0,10,1,1);
    titleLayout->addWidget(scrollDownButton,0,9,1,1);

    backButton->hide();
    //reloadButton->hide();
    loadingicon->hide();
    scrollDownButton->hide();

    mainLayout->addLayout(titleLayout,0,0);


    titleLineEdit = new QLineEdit();
    addressLineEdit = new QLineEdit();
    QPushButton *add_manga = new QPushButton("Add Series");
    //new_manga->addRow(tr("&Title:"), titleLineEdit);
    //new_manga->addRow(tr("&Address:"), addressLineEdit);
    auto nameLabel = new QLabel(tr("&Title:"));
    nameLabel->setBuddy(titleLineEdit);
    auto nameLabel2 = new QLabel(tr("&Address:"));
    nameLabel2->setBuddy(addressLineEdit);
    new_manga->addWidget(nameLabel, 0, 0);
    new_manga->addWidget(titleLineEdit, 0, 1);
    new_manga->addWidget(nameLabel2, 1, 0);
    new_manga->addWidget(addressLineEdit, 1, 1);
    new_manga->addWidget(add_manga,2,1);

    connect(add_manga, SIGNAL(released()), this, SLOT(add_new_manga()));

    new_manga_carrier->setLayout(new_manga);

    new_manga_carrier->hide();

    mainLayout->addWidget(new_manga_carrier);

    scrollArea = new QScrollArea;
    QWidget *resultsPage = new QWidget;
    buttonLayout = new QGridLayout;

    buttonLayout->setSpacing(0);

    resultsPage->setLayout(buttonLayout);

    scrollArea->setWidget(resultsPage);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    addButton->resize(backButton->size());
    //loadingicon->show();
    loadingmovie->start();
    //read the manga to track from file. not the best way to do this but it will do for now
    std::fstream myfile;

    myfile.open("../sav.json");
	nlohmann::json j;
	if(myfile.is_open()){
		myfile >> j;
	}

    for (auto& element : j) {
        std::string root;
        element.at("Root address").get_to(root);

        //std::cout << element["Root address"] << '\n';
        auto y=new manga;
        std::string temp;

        element.at("Title").get_to(temp);
        y->title=strdup(temp.c_str());
        element.at("Address").get_to(temp);
        y->address=strdup(temp.c_str());
        //element.at("Address").get_to(temp);

        element.at("Title").get_to(temp);

        for (auto x:element["Chapters"]) {
            std::string temp;
            x["Name"].get_to(temp);
            //std::cout<<temp<<std::endl;

            auto temp_chapter=new chapter;

            x["Name"].get_to(temp);
            temp_chapter->name=strdup(temp.c_str());

            std::string address = root+temp;
            temp_chapter->address=strdup(address.c_str());

            x["Read"].get_to(temp_chapter->read);

            y->chapters.emplace_back(temp_chapter);

        }
        //std::reverse(y->chapters.begin(), y->chapters.end());
        y->loaded=true;
        mangas.emplace(temp, y);
        //loader_queue.push_back(y);
    }

    myfile.close();

    deque_lock=false;

    curler = new std::thread(&MainWidget::manga_loader, this);
    //curler2 = new std::thread(&MainWidget::manga_loader, this);
	///todo multithread curl
    {
		//load additional sources
        std::fstream myfile;
        myfile.open ("../sources.txt");
        //myfile << "Writing this to a file.\n";
        std::string tp;
        bool everyother=false;
        std::string previous;
        while(getline(myfile, tp)){ //read data from file object and put it into string.
            //std::cout << tp << "\n"; //print the data of the string
            if(everyother){
                auto x=new manga;
                x->title=strdup(previous.c_str());
                x->address=strdup(tp.c_str());
                mangas.emplace(previous, x);
            }
            previous=tp;
            everyother=!everyother;
        }
        myfile.close();
    }
    for (auto x:mangas) {
        QPushButton* btr= new QRightClickButton(QString::fromStdString(x.second->title));
        ((QRightClickButton*)btr)->address= strdup(x.second->address);
        buttonLayout->addWidget(btr);
        connect(btr, SIGNAL(released()), this, SLOT(setValue()));

    }
    //add empty expanding widget to fill space at bottom of page. Aligns buttons to top of screen
    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    buttonLayout->addWidget(empty);
}
// Destructor
MainWidget::~MainWidget(){
    running=false;
    std::fstream myfile;
    //todo multithread curl
    //todo add system to move series you are trying to load to front of queue
    //todo add refresh button to refresh everything
    //todo periodicaly reload data for series
    //todo keep track of previously unsceen chapters and notify if new found
    std::cout<<"hi";
    curler->join();
    //curler2->join();
    nlohmann::json j;
    for (auto x:mangas) {
        j[x.second->title]={{"Title", x.second->title}, {"Address", x.second->address}};
        std::string root=std::string(x.second->chapters[0]->address).substr(0,std::string(x.second->chapters[0]->address).rfind("chapter"));
        j[x.second->title]["Root address"]=root;
        int i=0;
        for (auto y:x.second->chapters) {
            j[x.second->title]["Chapters"][i]={{"Name", y->name}, {"Read", y->read}};
            i++;
        }
    }
    std::remove("../sav.json");
    myfile.open("../sav.json", std::fstream::out);

    myfile << j.dump(1,'\t');

    myfile.close();
}
void MainWidget::setValue(){
    std::cout<<((QPushButton*)sender())->text().toLocal8Bit().constData()<<std::endl;
    auto qrsender=(QRightClickButton*)sender();

    title->setText(qrsender->text());
    backButton->show();
    reloadButton->show();
    scrollDownButton->show();
    scrollArea->hide();
    new_manga_carrier->hide();
    addButton->hide();
    subLayout = new QGridLayout;
    subScrollArea = new QScrollArea;

    subScrollArea->setLayout(subLayout);
    QWidget *resultsPage = new QWidget;

    manga* target=mangas.at(((QPushButton*)sender())->text().toStdString());
    if (!target->loaded){
        loader_queue.push_front(target);
    }
    while (!target->loaded){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout<<target->address<<std::endl<<std::endl;


    subLayout->setSpacing(0);

    resultsPage->setLayout(subLayout);

    subScrollArea->setWidget(resultsPage);
    subScrollArea->setBackgroundRole(QPalette::Dark);
    subScrollArea->setWidgetResizable(true);

    mainLayout->addWidget(subScrollArea);
    emit(reload_chapter_buttons_signal());

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    subLayout->addWidget(empty);

}
void MainWidget::clickChapter(){
    auto qrsender=(QRightClickButton*)sender();
    std::cout<<(qrsender->text().toLocal8Bit().constData())<<std::endl;
    qrsender->setFlat(true);
    int i=0;
    while(mangas.at(title->text().toStdString())->chapters[i]->name!=qrsender->text().toStdString()){
        i++;
    }
    mangas.at(title->text().toStdString())->chapters[i]->read=qrsender->isFlat();
    QString link =((QRightClickButton*)sender())->address;
    QDesktopServices::openUrl(QUrl(link));
}
void MainWidget::rightClickChapter() {
    auto qrsender=(QRightClickButton*)sender();
    std::cout<<qrsender->text().toStdString()<<std::endl;
    ((QPushButton*)sender())->setFlat(!qrsender->isFlat());
    //there has to be a better way of doing this
    int i=0;
    while(mangas.at(title->text().toStdString())->chapters[i]->name!=qrsender->text().toStdString()){
        i++;
    }
    mangas.at(title->text().toStdString())->chapters[i]->read=qrsender->isFlat();
}
void MainWidget::back_button_clicked() {
    while (auto item = subLayout->takeAt(0)) {
        subLayout->removeItem(item);
        delete item->widget();
        delete item;
    }
    subScrollArea->hide();
    scrollArea->show();
    addButton->show();
    backButton->hide();
    scrollDownButton->hide();
    //Button->hide();
    title->setText("Webtrakr");
}
void MainWidget::add_button_clicked() {
    new_manga_carrier->show();
}
void MainWidget::add_new_manga() {
    auto x = new manga;
    x->title=strdup(titleLineEdit->text().toStdString().c_str());
    x->address=strdup(addressLineEdit->text().toStdString().c_str());
    mangas.emplace(titleLineEdit->text().toStdString(), x);
    loader_queue.push_back(x);

    titleLineEdit->setText("");
    addressLineEdit->setText("");

    new_manga_carrier->hide();

    QPushButton* btr= new QRightClickButton(QString::fromStdString(x->title));
    ((QRightClickButton*)btr)->address= strdup(x->address);
    buttonLayout->addWidget(btr);
    connect(btr, SIGNAL(released()), this, SLOT(setValue()));
}
void MainWidget::reload_manga(){
    //auto qrsender=(QRightClickButton*)sender();
    //std::cout<<mangas.at(title->text().toStdString())->title<<std::endl;
    while(deque_lock){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    deque_lock=true;
    if(title->text().toStdString()=="Webtrakr"){
        std::cout<<"main page"<<std::endl;
        for (auto x:mangas) {
            loader_queue.push_back(x.second);
        }
    }
    else{
        loader_queue.push_front(mangas.at(title->text().toStdString()));
    }
    deque_lock=false;
}
void MainWidget::reload_chapter_buttons(){
    while (auto item = subLayout->takeAt(0)) {
        subLayout->removeItem(item);
        delete item->widget();
        delete item;
    }
    for (auto x:mangas.at(title->text().toStdString())->chapters) {
        QPushButton* btr= new QRightClickButton(QString::fromStdString(x->name));
        ((QRightClickButton*)btr)->title= strdup(title->text().toStdString().c_str());
        ((QRightClickButton*)btr)->address= strdup(x->address);
        btr->resize(scrollArea->size().width(),50);
        btr->setFlat(x->read);

        subLayout->addWidget(btr);
        connect(btr, SIGNAL(released()), this, SLOT(clickChapter()));
        connect(btr, SIGNAL(rightClicked()), this, SLOT(rightClickChapter()));
    }
    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    subLayout->addWidget(empty);
    //subScrollArea->verticalScrollBar()->setValue(subScrollArea->verticalScrollBar()->maximum());
    manga* cur=mangas.at(title->text().toStdString());
    if (chapter_num_from_str(cur->chapters[0]->name) > 1 ){
        if(!cur->chapters[0]->read){
            scrollDown();
        }
    }
    else{
        if(cur->chapters[0]->read){
            scrollDown();
        }
    }

}
void MainWidget::scrollDown(){
    manga* cur=mangas.at(title->text().toStdString());
    bool start=cur->chapters[0]->read;

    int chapter_index=cur->chapters.size();
    for (int i = 0; i < cur->chapters.size(); ++i) {
        if (cur->chapters[i]->read!=start){
            chapter_index=i;
            break;
        }
    }
    subScrollArea->verticalScrollBar()->setValue(subScrollArea->verticalScrollBar()->maximum()*chapter_index/cur->chapters.size());

    //subScrollArea->verticalScrollBar()->setValue(subScrollArea->verticalScrollBar()->maximum());

}