#include <bits/stdc++.h>

using namespace std;

//工厂模式核心思想：将对象的创建过程封装在一个工厂类中，通过调用工厂类的方法来创建对象，而不是直接在代码中创建对象。
//核心的几个类：抽象产品类、具体产品类、抽象工厂类、具体工厂类。

//共有三种工厂模式：简单工厂模式、工厂方法模式和抽象工厂模式。

//简单工厂模式：定义一个工厂类，根据传入的参数来创建不同的产品对象;所有对象创建都是一个工厂类来创建的；适用于产品种类较少的情况。
class Product {
public:
    virtual void use() = 0;
    virtual ~Product() {}
};

class ProductA : public Product {
public:
    void use() override {
        cout << "use ProductA" << endl;
    }
};

class ProductB : public Product {
public:
    void use() override {
        cout << "use ProductB" << endl;
    }
};

class SimpleFactory {
public:
    static Product* createProduct(string type) {
        if (type == "A") {
            return new ProductA();
        } else if (type == "B") {
            return new ProductB();
        } else {
            cout << "Unknown type" << endl;
            return nullptr;
        }
    }
};


//工厂方法模式：定义一个抽象工厂类，每个具体工厂类都继承自抽象工厂类，并实现自己的创建产品的方法；
//每个具体工厂类都创建一种产品对象；适用于产品种类较多的情况。

class Factory {
public:
    virtual Product* createProduct() = 0;
    virtual ~Factory() {}
};

class FactoryA : public Factory {
public:
    Product* createProduct() override {
        return new ProductA();
    }
};

class FactoryB : public Factory {
public:
    Product* createProduct() override {
        return new ProductB();
    }
};

//抽象工厂模式：定义一个抽象工厂类，每个具体工厂类都继承自抽象工厂类，并实现自己的创建产品的方法。每个具体工厂类都创建一组相关的产品对象。

class ConcreteProductA1 : public ProductA {
public:
    void use() override {
        cout << "use ConcreteProductA1" << endl;
    }
};

class ConcreteProductA2 : public ProductA {
public:
    void use() override {
        cout << "use ConcreteProductA2" << endl;
    }
};

class ConcreteProductB1 : public ProductB {
public:
    void use() override {
        cout << "use ConcreteProductB1" << endl;
    }
};

class ConcreteProductB2 : public ProductB {
public:
    void use() override {
        cout << "use ConcreteProductB2" << endl;
    }
};

class AbstractFactory {

public:
    virtual ProductA* createProductA() = 0;
    virtual ProductB* createProductB() = 0;
    virtual ~AbstractFactory() {}
};

class ConcreteFactory1 : public AbstractFactory {
public:
    ProductA* createProductA() override {
        return new ConcreteProductA1();
    }
    ProductB* createProductB() override {
        return new ConcreteProductB1();
    }
};

class ConcreteFactory2 : public AbstractFactory {
public:
    ProductA* createProductA() override {
        return new ConcreteProductA2();
    }
    ProductB* createProductB() override {
        return new ConcreteProductB2();
    }
};

int main() {
    //简单工厂模式, 直接使用工厂类创建所有的产品
    SimpleFactory* factory = new SimpleFactory();
    auto productA = factory->createProduct("A");
    productA->use();
    delete factory;

    //工厂方法模式，使用具体工厂类创建具体产品
    Factory* factory1 = new FactoryA();
    auto productA1 = factory1->createProduct();
    productA1->use();
    delete factory1;

    //抽象工厂模式，使用具体工厂类创建一系列相关产品
    AbstractFactory* factory2 = new ConcreteFactory1();
    auto productA2 = factory2->createProductA();
    productA2->use();
    delete factory2;
}