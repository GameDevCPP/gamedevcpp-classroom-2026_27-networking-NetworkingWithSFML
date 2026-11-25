# Networking: Host and Join

Five executables: bare console TCP (server/client), bare console UDP
(server/client), and a host-and-join demo wired into this course's ECM.

Read the code first (it's short), run it, then extend it. See "Tasks"
below.

## What's here

- **`NetworkServer`** / **`NetworkClient`** -- bare console TCP, no ECM.
  Run `NetworkServer` first, then `NetworkClient`; they exchange one
  `sf::Packet` each way and print what they got.
- **`NetworkServerUDP`** / **`NetworkClientUDP`** -- bare console UDP, no
  ECM. No connect step -- the client just sends straight to an address
  and port, the server just binds and waits. Low-level `send`/`receive`
  here (raw buffer, not `sf::Packet`).
- **`NetworkDemo`** -- host-and-join over TCP, wired into the ECM. See
  below.

## Running NetworkDemo

Run two instances (two terminals, or two separate machines on the same
network). One picks Host, the other Join:

```
Host or Join? [h/j]: h
```
```
Host or Join? [h/j]: j
Host IP [127.0.0.1]: 127.0.0.1
```

Move the cyan circle with arrow keys. The orange circle is the other
player -- it only ever moves from network data, never local input.

Only works over localhost, a shared hotspot, or a LAN both machines are
genuinely on. College/eduroam-style WiFi almost always blocks
device-to-device traffic (AP client isolation) regardless of TCP vs UDP --
that's a WiFi policy, not something fixable in code. To host across the
actual internet (not just LAN), you'd need a public-facing host: a cloud
VM (e.g. AWS Lightsail) with the listening port open in its firewall,
or port-forwarding on the host's own router. The code doesn't care where
the other end is -- `Join`'s IP field takes any reachable address.

## Where the networking code lives

- `engine/system_network.h/.cpp` -- a free-standing system, the same shape
  as `engine/system_physics.h`: global connection state, not per-entity.
  A TCP connection is a session-wide resource, so it lives where the rest
  of this engine keeps session-wide things.
- `src/NetworkDemo/components/cmp_network.h/.cpp` -- the per-entity
  piece. `Role::Local` sends this entity's position every frame;
  `Role::Remote` applies whatever arrives. No socket code in the
  component at all -- it only talks to `Network::`.
- `src/NetworkDemo/scenes/scene_network_demo.cpp` -- spawns one of each:
  a `Local` entity you move with arrow keys, a `Remote` "ghost" entity
  that's purely network-driven.

If you want networking in your own game, this is the split to copy: one
system owning the connection, one thin component per entity that's
actually replicated.

## A real bug worth knowing about

`Network::sendLocalPosition` resends a packet on `Socket::Partial` instead
of just firing-and-forgetting:

```cpp
Socket::Status status;
do {
  status = _socket.send(packet);
} while (status == Socket::Partial);
```

On a non-blocking socket, `send(Packet&)` can return `Partial`: the OS
didn't take the whole packet in one go. TCP has no message boundaries, so
a half-sent packet corrupts the framing for *every packet after it, for
the rest of the connection* -- not just that one update. Ignoring the
return value (the first version of this code did) looked perfectly fine
on localhost, where the loopback buffer never fills, and then broke in
exactly one direction the first time it ran over real WiFi between two
actual machines, because that's where the buffer can actually fill under
sustained load. If you hit something like "one side moves fine, the other
side's updates never arrive, and it never recovers," this is almost
certainly why.

## Tasks

`NetworkDemo` only ever syncs one `Vector2f`, over TCP. Some directions to
take it further:

- Sync rotation too, not just position.
- Add a second networked entity per side (e.g. a thrown object) --
  this means `NetworkComponent` needs some way to tell entities apart,
  since right now there's exactly one Local and one Remote.
- Right now `startHost` only ever accepts one client. Support a second
  joiner (three total players).
- Rebuild `NetworkDemo`'s position sync on top of `NetworkServerUDP`/
  `NetworkClientUDP` instead of TCP. You'll need your own framing (UDP
  packets can arrive reordered or not at all, which TCP's stream
  abstraction hides from you) and your own retry logic for anything that
  must arrive. Compare how much extra code that costs against what
  `sf::Packet` + TCP gives you for free.
