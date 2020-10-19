#include "../sylar/config.h"
#include "../sylar/log.h"
#include<yaml-cpp/yaml.h>
sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup("system.port", (int)8080,"system port");
sylar::ConfigVar<float>::ptr g_int_valuex_config = sylar::Config::Lookup("system.port", (float)8080,"system port");


sylar::ConfigVar<float>::ptr g_float_value_config = sylar::Config::Lookup("system.value",(float)10.2f,"system value");


sylar::ConfigVar<std::vector<int> >::ptr g_int_vector_value_config = sylar::Config::Lookup("system.int_vec",std::vector<int>{1,2},"system int vec");

sylar::ConfigVar<std::list<int> >::ptr g_int_list_value_config = sylar::Config::Lookup("system.int_list",std::list<int>{1,2},"system int list");

sylar::ConfigVar<std::set<int> >::ptr g_int_set_value_config = sylar::Config::Lookup("system.int_set",std::set<int>{1,2},"system int set");

sylar::ConfigVar<std::unordered_set<int> >::ptr g_int_unordered_set_value_config = sylar::Config::Lookup("system.unordered_set",std::unordered_set<int>{1,2},"system int set");

sylar::ConfigVar<std::map<std::string ,int> >::ptr g_int_map_value_config = sylar::Config::Lookup("system.map",std::map<std::string,int>{{"key",2}},"system int set");

sylar::ConfigVar<std::unordered_map<std::string ,int> >::ptr g_int_unordered_map_value_config = sylar::Config::Lookup("system.umap",std::unordered_map<std::string,int>{{"key",2}},"system int set");

void print_yaml(YAML::Node const &node, int level){
    if(node.IsScalar()){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4,' ')<<node.Scalar() << " - " <<node.Type() << " - "<<level;
    }
    else if(node.IsNull()){
         SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< std::string(level * 4,' ') << "NULL" << " - " <<node.Type() << " - "<<level;
    }
    else if (node.IsMap()){
        for(auto it = node.begin();it != node.end();++it){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< std::string(level * 4,' ') << it->first << " - " <<it->second.Type() << " - "<<level;
            print_yaml(it->second,level+1);
        }
    }
    else if(node.IsSequence()){
            //for(auto it = node.begin();it != node.end();++it){
            //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << it->first << " - " <<it->second.Tag() << " - "<<level;
            for(size_t i = 0; i< node.size();++i){
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< std::string(level * 4,' ') << i << " - " <<node[i].Type() << " - "<<level;
                //YAML::Node& hello = node[i];
                print_yaml(node[i],level+1);
            }
            
    }

}
void test_yaml(){
    //加载yaml
    YAML::Node root = YAML::LoadFile("/root/workspace/bin/conf/log.yml");
    print_yaml(root,0);
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;
}

void testconfig(){
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before:"<< g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before:"<<g_float_value_config->toString();
    #define XX(g_var,name,prefix) \
    { \
    auto& v = g_var ->getValue(); \
    for(auto&i :v){ \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< #prefix " " #name" : " << i; \
    } \
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< #prefix " " #name" yaml: " << g_var->toString(); \
    }

    #define XX_M(g_var,name,prefix) \
    { \
    auto& v = g_var ->getValue(); \
    for(auto&i :v){ \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< #prefix " " #name" : {" << i.first << " - " << i.second << "}"; \
    } \
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<< #prefix " " #name" yaml: " << g_var->toString(); \
    }

    XX(g_int_vector_value_config,int_vec,before);
    XX(g_int_list_value_config,g_list,before);
    XX(g_int_set_value_config,g_set,before);
    XX(g_int_unordered_set_value_config,g_unordered_set,before);
    XX_M(g_int_map_value_config,g_map,before);
    XX_M(g_int_unordered_map_value_config ,g_umap,before);

    YAML::Node root = YAML::LoadFile("/root/workspace/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"after:"<< g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after:"<<g_float_value_config->toString();

    XX(g_int_vector_value_config,int_vec,after);
    XX(g_int_list_value_config,int_list,after);
    XX(g_int_set_value_config,int_set,after);
    XX(g_int_unordered_set_value_config,g_unordered_set,after);
    XX_M(g_int_map_value_config,g_map,after);
    XX_M(g_int_unordered_map_value_config ,g_umap,after);
}
class Person{
public:
    Person(){}
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;
    std::string toString() const{
        std::stringstream ss;
        ss  <<"[Person name = "<<m_name
            <<" age="<<m_age
            <<" sex="<<m_sex
            <<"]";
        return ss.str();
    }

    bool operator==(const Person& oth) const{
        return m_name ==oth.m_name&&
                m_age ==oth.m_age&&
                m_sex==oth.m_sex;
    }
};
namespace sylar{
template<>
class LexicalCast<std::string, Person>{
public:
    Person operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        Person p;
        //typename std::unordered_map<std::string, T> vec;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
        
    }
};

template<>
class LexicalCast<Person ,std::string >{
public:
    std::string operator()(const Person& v){
        YAML::Node node;
        node["name"] = v.m_name;
        node["age"] = v.m_age;
        node["sex"] = v.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}
sylar::ConfigVar<Person>::ptr g_person = sylar::Config::Lookup("class.person", Person(),"system person");
void test_class(){
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before: " << g_person->getValue().toString() << " - " << g_person->toString();
    YAML::Node root = YAML::LoadFile("/root/workspace/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"after: " << g_person->getValue().toString() << " - " << g_person->toString();
}
int main(){
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_config->getValue();
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->toString();
    //test_yaml();
    //testconfig();
    test_class();
    return 0;
}