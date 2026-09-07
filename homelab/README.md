# Pintoserve Homelab Setup

Domain: **pintoserve.com**  
Server: Unraid (Docker via Compose Manager Plus)

## Todo

- [x] Unraid Docker enabled (`appdata` + `system` on cache)
- [x] Immich installed and running (LAN access + mobile app)
- [ ] Expose Immich at a public URL on `pintoserve.com` (Cloudflare Tunnel)
- [ ] PhotoPrism stack (read-only browse/search over Immich library)
- [ ] Ofelia scheduler (nightly PhotoPrism index + convert/backup jobs)
- [ ] Backups for Immich library + databases
- [ ] (Optional) Cloudflare Access lock in front of Immich

---

## Immich public URL on pintoserve.com

**Recommended path:** Cloudflare Tunnel (no router port forwards).

Suggested hostname: `photos.pintoserve.com` → Immich on Unraid (`http://UNRAID-LAN-IP:2283`)

### Why this path
- No open ports on your home router
- Keeps your home IP off public DNS
- HTTPS handled by Cloudflare
- Easy to add PhotoPrism later (`prism.pintoserve.com`, etc.)

### Steps

#### 1. Put the domain on Cloudflare DNS
1. Create a free account at [dash.cloudflare.com](https://dash.cloudflare.com)
2. **Add site** → enter `pintoserve.com`
3. Choose the **Free** plan
4. Cloudflare shows two nameservers (e.g. `ada.ns.cloudflare.com`)
5. At your domain registrar, replace the domain’s nameservers with Cloudflare’s
6. Wait until Cloudflare shows the zone as **Active** (can take from minutes to a few hours)

#### 2. Create a Tunnel
1. Cloudflare dashboard → **Zero Trust** (free team is fine)
2. **Networks → Tunnels → Create a tunnel**
3. Pick **Cloudflared**
4. Name it something like `unraid-pintoserve`
5. Copy the **tunnel token** (long string) — keep it private

#### 3. Run `cloudflared` on Unraid
**Apps** → search **Cloudflared** / **Cloudflare Tunnel** → install.

Set the token from step 2 in the container template (often `TOKEN` or in the command).

Start the container and confirm it shows **Healthy / Connected** in the Cloudflare Tunnels UI.

#### 4. Route the hostname to Immich
In the tunnel’s **Public Hostname** tab:

| Field | Value |
|--------|--------|
| Subdomain | `photos` |
| Domain | `pintoserve.com` |
| Path | (leave empty) |
| Service type | `HTTP` |
| URL | `http://UNRAID-LAN-IP:2283` |

Use your Unraid server’s LAN IP (the same one that works from your phone), **not** `localhost` from Cloudflare’s perspective unless cloudflared is using host networking and Immich is on that host.

Save. Cloudflare will create the DNS CNAME for `photos.pintoserve.com` automatically.

#### 5. Point Immich at the public URL
In Immich (web) → **Administration → Settings → Server Settings** (wording may vary by version):

- Set the public / external domain to: `https://photos.pintoserve.com`

Also update phone app server URL to:

```text
https://photos.pintoserve.com
```

(Use `https`, no port.)

#### 6. Lock it down (strongly recommended)
Immich has its own login, but scanners will still find the hostname.

In Cloudflare Zero Trust → **Access → Applications**:

1. Add an application for `photos.pintoserve.com`
2. Policy: allow your email (One-time PIN / Google / GitHub)
3. Optionally exclude Immich API paths later if the mobile app has trouble with Access — many people start with Access on, and if the app breaks, either:
   - use a bypass policy for the mobile app’s needs, or
   - rely on Immich auth only and keep the Tunnel (still no open ports)

Test in a browser first, then the Immich app.

#### 7. Upload / performance note
Large phone backups are often faster on home Wi‑Fi using the LAN URL (`http://UNRAID-IP:2283`). Public URL is best for remote browsing/sharing; LAN for bulk backup at home.

---

## Next: PhotoPrism + Ofelia

After Immich is reachable via the domain:

1. Find Immich library UUID under `/mnt/user/photos/immich/library/`
2. Deploy PhotoPrism with that path mounted **read-only** under `/photoprism/originals/Immich`
3. Add Ofelia with `jobs.ini` to run `photoprism index --cleanup` on a schedule
4. Keep Immich as the upload/source-of-truth app

---

## Useful local paths

| Path | Purpose |
|------|---------|
| `/mnt/user/photos/immich` | Immich `UPLOAD_LOCATION` |
| `/mnt/cache/appdata/immich/postgres` | Immich DB (direct cache path) |
| `/mnt/user/appdata/photoprism/` | PhotoPrism storage/DB/Ofelia (planned) |

---

## Security reminders
- Do **not** port-forward Immich or the Unraid UI
- Do **not** expose Unraid’s management ports on the tunnel
- Treat `pintoserve.com` hostnames as public once DNS is live
- Keep Immich, cloudflared, and Unraid updated
