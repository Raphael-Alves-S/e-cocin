#ifndef ECOCIN_ENTITIES_CLIENT_H
#define ECOCIN_ENTITIES_CLIENT_H

#include <string>
#include "core/Time.h"

class Client{
    private:
        long long id_;
        std::string name_;
        std::string email_;
        std::string cpf_;
        ecocin::core::Timestamp createDate_;
    
        //Construtores
    public:
        Client();
        Client(const std::string& name,
               const std::string& email,
               const std::string& cpf);


        //getters
        long long                 getId() const {return id_;}
        const std::string&        getName() const {return name_;}
        const std::string&        getEmail() const {return email_;}
        const std::string&        getCpf()   const {return cpf_;}
        ecocin::core::Timestamp   getCreateDate() const {return createDate_;}

        //setters
        void setId(long long id)                            {id_ = id;}
        void setName(const std::string& name)              { name_ = name; }
        void setEmail(const std::string& email)            { email_ = email; }
        void setCpf(const std::string& cpf)                { cpf_ = cpf; }
        void setCreateDate(ecocin::core::Timestamp createDate) { createDate_ = createDate; }

        // Utilitários leves (opcionais): normalizações simples
        static std::string trim(const std::string& s);


        




};

#endif
