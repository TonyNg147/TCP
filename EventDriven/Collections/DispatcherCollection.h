#include <Dispatcher/Dispatcher.h>
#include <SystemType/DispatcherKind.h>

#include <unordered_map>
class DispatcherCollection {
public:
	static DispatcherCollection &getInstance();

	DispatcherCollection();

	~DispatcherCollection();

	Dispatcher *getSource(const DispatcherKind &) const;

private:
	std::unordered_map<DispatcherKind, Dispatcher *> m_dispatchers;
};