import pytest
import shlex
import time


@pytest.mark.skipif('Dot3' not in pytest.config.lldpd.features,
                    reason="Dot3 not supported")
class TestLldpDot3(object):

    def test_aggregate(self, lldpd1, lldpd, lldpcli, namespaces, links):
        links(namespaces(3), namespaces(2))  # Another link to setup a bond
        with namespaces(2):
            idx = links.bond('bond42', 'eth1', 'eth3')
            lldpd()
        with namespaces(1):
            out = lldpcli("-f", "keyvalue", "show", "neighbors", "details")
            assert out['lldp.eth0.port.descr'] == 'eth1'
            assert out['lldp.eth0.port.aggregation'] == str(idx)

    # TODO: unfortunately, with veth, it's not possible to get an
    # interface with autoneg.

    @pytest.mark.parametrize("command, expected", [
        ("pse supported enabled paircontrol powerpairs spare class class-3",
         {'supported': 'yes',
          'enabled': 'yes',
          'paircontrol': 'yes',
          'device-type': 'PSE',
          'pairs': 'spare',
          'class': 'class 3'}),
        ("pd supported enabled powerpairs spare class class-3 typeat 1 source "
         "pse priority low requested 10000 allocated 15000",
         {'supported': 'yes',
          'enabled': 'yes',
          'paircontrol': 'no',
          'device-type': 'PD',
          'pairs': 'spare',
          'class': 'class 3',
          'power-type': '1',
          'source': 'Primary power source',
          'priority': 'low',
          'requested': '10000',
          'allocated': '15000'}),
         #Single signature typebt
        ("pd powerpairs signal class class-4 typeat 2 source pse priority low requested 60000 "
         "allocated 59000 typebt 4single pid4 supported pdStatus single classExt "
         "class-8 aClass single bClass single pdLoad not-isolated autoclassRequest",
         {'supported': 'no',
          'enabled': 'no',
          'paircontrol': 'no',
          'device-type': 'PD',
          'pairs': 'signal',
          'class': 'class 4',
          'power-type': '2',
          'source': 'Primary power source',
          'priority': 'low',
          'requested': '60000',
          'allocated': '59000',
          'pid4': '4PID is supported by PD',
          'aRequested': '0',
          'bRequested': '0',
          'aAllocated': '0',
          'bAllocated': '0',
          'pseStatus': 'unknown',
          'pdStatus': 'Powered single-signature PD',
          'pairsExt': 'unknown',
          'aClass': 'Single-signature PD or 2-pair only PSE',
          'bClass': 'Single-signature PD or 2-pair only PSE',
          'classExt': 'Class 8',
          'typebt': 'Type 4 single-signature PD',
          'pdLoad':'PD is single-signature or dual-signature and power demand on Mode A and Mode B are not electrically isolated.',
          'pseMaxPower':'0',
          'autoclassSupport':'PSE does not support Autoclass',
          'autoclassComplete':'Autoclass idle',
          'autoclassRequest':'PD requests Autoclass measurement',
          'powerDownRequest':'unknown',
          'powerDownTime':'0'}),
         #Dual signature typebt
         ("pd powerpairs signal typeat 2 source pse priority low requested 0 "
         "allocated 0 typebt 4dual aRequested 20000 bRequested 30000 "
         "aAllocated 40000 bAllocated 45000 pdStatus dual4pair pid4 "
         "supported aClass class-4 bClass class-5 classExt dual-signature-pd "
         "powerDownRequest powerDownTime 130",
         {'supported': 'no',
          'enabled': 'no',
          'paircontrol': 'no',
          'device-type': 'PD',
          'pairs': 'signal',
          'class': 'unknown',
          'power-type': '2',
          'source': 'Primary power source',
          'priority': 'low',
          'requested': '0',
          'allocated': '0',
          'pid4': '4PID is supported by PD',
          'aRequested': '20000',
          'bRequested': '30000',
          'aAllocated': '40000',
          'bAllocated': '45000',
          'pseStatus': 'unknown',
          'pdStatus': '4-pair powered dual-signature PD',
          'pairsExt': 'unknown',
          'aClass': 'Class 4',
          'bClass': 'Class 5',
          'classExt': 'Dual-signature PD',
          'typebt': 'Type 4 dual-signature PD',
          'pdLoad':'PD is single-signature or dual-signature and power demand on Mode A and Mode B are not electrically isolated.',
          'pseMaxPower':'0',
          'autoclassSupport':'PSE does not support Autoclass',
          'autoclassComplete':'Autoclass idle',
          'autoclassRequest':'Autoclass idle',
          'powerDownRequest':'PD requests a power down',
          'powerDownTime':'130'})])
    def test_power(self, lldpd1, lldpd, lldpcli, namespaces,
                   command, expected):
        with namespaces(2):
            lldpd()
            result = lldpcli(
                *shlex.split("configure dot3 power {}".format(command)))
            assert result.returncode == 0
            time.sleep(3)
        with namespaces(1):
            pfx = "lldp.eth0.port.power."
            out = lldpcli("-f", "keyvalue", "show", "neighbors", "details")
            out = {k[len(pfx):]: v
                   for k, v in out.items()
                   if k.startswith(pfx)}
            assert out == expected

    def test_autoneg_power(self, links, lldpd, lldpcli, namespaces):
        links(namespaces(1), namespaces(2))
        with namespaces(1):
            lldpd()
        with namespaces(2):
            lldpd()
            result = lldpcli(
                *shlex.split("configure dot3 power pd "
                             "supported enabled paircontrol "
                             "powerpairs spare "
                             "class class-3 "
                             "type 1 source both priority low "
                             "requested 20000 allocated 5000"))
            assert result.returncode == 0
            time.sleep(2)
        with namespaces(1):
            # Did we receive the request?
            out = lldpcli("-f", "keyvalue", "show", "neighbors", "details")
            assert out['lldp.eth0.port.power.requested'] == '20000'
            assert out['lldp.eth0.port.power.allocated'] == '5000'
            # Send an answer we agree to give almost that (this part
            # cannot be automated, lldpd cannot take this decision).
            result = lldpcli(
                *shlex.split("configure dot3 power pse "
                             "supported enabled paircontrol powerpairs "
                             "spare class class-3 "
                             "type 1 source primary priority high "
                             "requested 20000 allocated 19000"))
            assert result.returncode == 0
            time.sleep(2)
        with namespaces(2):
            # Did we receive that?
            out = lldpcli("-f", "keyvalue", "show", "neighbors", "details")
            assert out['lldp.eth1.port.power.requested'] == '20000'
            assert out['lldp.eth1.port.power.allocated'] == '19000'
        with namespaces(1):
            # Did we get an echo back? This part is handled
            # automatically by lldpd: we confirm we received the
            # answer "immediately".
            out = lldpcli("-f", "keyvalue", "show", "neighbors", "details")
            assert out['lldp.eth0.port.power.requested'] == '20000'
            assert out['lldp.eth0.port.power.allocated'] == '19000'
