//
//  Attributes.hpp
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Cpu6502_Attributes_hpp
#define VisualUnitTest_Cpu6502_Attributes_hpp

#include "../Attribute.hpp"


namespace VisualUnitTest { namespace Cpu6502 {
    
    template <class TCpu6502>
    struct AttributeAb : Attribute<TCpu6502> {
        AttributeAb(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint16_t _ab;
    };
    
    template <class TCpu6502>
    struct AttributeDb : Attribute<TCpu6502> {
        AttributeDb(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _db;
    };
    
    template <class TCpu6502>
    struct AttributeRw : Attribute<TCpu6502> {
        AttributeRw(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        bool _rw;
    };
    
    template <class TCpu6502>
    struct AttributePc : Attribute<TCpu6502> {
        AttributePc(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint16_t _pc;
    };
    
    template <class TCpu6502>
    struct AttributePcl : Attribute<TCpu6502> {
        AttributePcl(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _pcl;
    };
    
    template <class TCpu6502>
    struct AttributePch : Attribute<TCpu6502> {
        AttributePch(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _pch;
    };
    
    template <class TCpu6502>
    struct AttributeA : Attribute<TCpu6502> {
        AttributeA(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _a;
    };
    
    template <class TCpu6502>
    struct AttributeX : Attribute<TCpu6502> {
        AttributeX(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _x;
    };
    
    template <class TCpu6502>
    struct AttributeY : Attribute<TCpu6502> {
        AttributeY(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _y;
    };
    
    template <class TCpu6502>
    struct AttributeS : Attribute<TCpu6502> {
        AttributeS(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _s;
    };
    
    template <class TCpu6502>
    struct AttributeP : Attribute<TCpu6502> {
        AttributeP(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _p;
    };
    
    template <class TCpu6502>
    struct AttributeSync : Attribute<TCpu6502> {
        AttributeSync(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        bool _sync;
    };
    
    template <class TCpu6502>
    struct AttributeReadyLine : Attribute<TCpu6502> {
        AttributeReadyLine(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        bool _readyLine;
    };
    
    template <class TCpu6502>
    std::unique_ptr<Attribute<TCpu6502>> attributeFactory(std::string const &name);
    
    #include "Attributes_s.hpp"
    
} }

#endif /* VisualUnitTest_Cpu6502_Attributes_hpp */
