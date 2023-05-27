#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <limits>
#include <vector>
#include <iostream>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};

ostream& operator<<(ostream& os, const Email& email)
{
  os << email.from << '\n' << email.to << '\n' << email.body << '\n';
  return os;
}

istream& operator>>(istream& is, Email& email)
{
  getline(is, email.from);
  getline(is, email.to);
  getline(is, email.body);
  return is;
}

class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const
  {
    if (!m_worker)
      return;

    m_worker->Process(move(email));
  }

public:
  void SetNext(unique_ptr<Worker> next)
  {
    m_worker = move(next);
  }

private:
  unique_ptr<Worker> m_worker;
};


class Reader : public Worker {
public:
  Reader(istream& is) : m_is(is) {}

  void Run() override
  {
    Email email;
    while (m_is >> email)
    {
      PassOn(unique_ptr<Email>(new Email(move(email))));
    }
  }

  void Process(unique_ptr<Email> email) override {}

private:
  istream& m_is;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

public:
  Filter(Function func) : m_func(func) {}

  void Process(unique_ptr<Email> email) override
  {
    if (!m_func(*email))
      return;

    PassOn(move(email));
  }

private:
  Function m_func;
};


class Copier : public Worker {
public:
  Copier(string address) : m_address(move(address)) {}

  void Process(unique_ptr<Email> email) override
  {
    unique_ptr<Email> emailCopy;
    if (email->to != m_address)
    {
      emailCopy = make_unique<Email>(*email);
      emailCopy->to = move(m_address);
      PassOn(move(email));
      PassOn(move(emailCopy));
    }
    else
    {
      PassOn(move(email));
    }
  }

private:
  string m_address;
};


class Sender : public Worker {
public:
  Sender(ostream& os) :m_os(os) {}

  void Process(unique_ptr<Email> email) override
  {
    m_os << *email;

    PassOn(move(email));
  }

private:
  ostream& m_os;
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in)
  {
    m_pipeline.push_back(make_unique<Reader>(in));
  }

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter)
  {
    m_pipeline.push_back(make_unique<Filter>(filter));
    return *this;
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient)
  {
    m_pipeline.push_back(make_unique<Copier>(move(recipient)));
    return *this;
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out)
  {
    m_pipeline.push_back(make_unique<Sender>(out));
    return *this;
  }

  // строит цепочку с помощью метода Worker::SetNext и возвращает первый обработчик в построенной цепочке
  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build()
  {
    for (size_t i = m_pipeline.size() - 1; i > 0; --i)
    {
      m_pipeline[i - 1]->SetNext(move(m_pipeline[i]));
    }

    return move(m_pipeline.front());
  }

private:
  vector<unique_ptr<Worker>> m_pipeline;
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
    );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
    });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
    );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
