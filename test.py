from build.plugin_lib import World

t = World()
t.set("bom dia!")
print(t.greet())

t.many(['Good Morning', 'Buon giorno', 'Kali mera'])
print(t.greet())