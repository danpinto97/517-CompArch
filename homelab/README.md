# Pintoserve Homelab Setup

Domain: **pintoserve.com**  
Server: Unraid (Docker via Compose Manager Plus)

## Todo

- [x] Unraid Docker enabled (`appdata` + `system` on cache)
- [x] Immich installed and running (LAN access + mobile app)
- [ ] Expose Immich at `https://photos.pintoserve.com` (Cloudflare Tunnel + Immich login only)
- [ ] PhotoPrism stack (read-only browse/search over Immich library)
- [ ] Ofelia scheduler (nightly PhotoPrism index + convert/backup jobs)
- [ ] Backups for Immich library + databases
- [ ] Immich app: local + external URL switching
- [ ] (Optional later) Cloudflare Access + service-token headers for mobile

---

## Immich public URL (chosen setup)

Same model as a typical shared app on a friend’s server:

```text
Internet → HTTPS → Cloudflare Tunnel → Immich username/password
```

- **No router port forwards**
- **No Cloudflare Access email gate** (Access breaks the Immich app unless you add service-token headers)
- **Immich accounts** control who can log in (create users in Immich admin to share with friends)

Suggested hostname: `photos.pintoserve.com` → `http://UNRAID-LAN-IP:2283`

### Steps

#### 1. Domain on Cloudflare
Already done (`pintoserve.com` via Cloudflare Registrar).

#### 2. Tunnel + cloudflared on Unraid
1. Zero Trust → **Networks → Tunnels → Create a tunnel** → Cloudflared
2. Name it (e.g. `unraid-pintoserve`) and copy the **token**
3. Run cloudflared on Unraid (Compose Manager Plus recommended):

```yaml
services:
  cloudflared:
    image: cloudflare/cloudflared:latest
    container_name: cloudflared
    restart: unless-stopped
    command: tunnel --no-autoupdate run
    environment:
      - TUNNEL_TOKEN=paste-your-token-here
```

4. Confirm tunnel status is **Healthy** in Cloudflare

#### 3. Public hostname → Immich
In the tunnel → **Public Hostname**:

| Field | Value |
|--------|--------|
| Subdomain | `photos` |
| Domain | `pintoserve.com` |
| Path | (empty) |
| Type | HTTP |
| URL | `http://UNRAID-LAN-IP:2283` |

#### 4. Remove Access from this hostname
If you created an Access application for `photos.pintoserve.com`, **delete or disable it**.

You do **not** need the email policy for this setup. Leave Access unused for Immich.

Browser and Immich app should both hit Immich’s own login page (JSON API), not a Cloudflare `<!DOCTYPE html>` interstitial.

#### 5. Immich server URL settings
In Immich web → **Administration → Settings**:

- External / public URL: `https://photos.pintoserve.com`

#### 6. Phone app
Server URL:

```text
https://photos.pintoserve.com
```

Log in with your Immich username/password (the account you created in Immich — not Cloudflare).

Optional but recommended: enable **local network switching**

- External: `https://photos.pintoserve.com`
- Local: `http://UNRAID-LAN-IP:2283`

Large video backups work better on LAN (Cloudflare Free has upload size limits).

#### 7. Sharing with someone else
1. Immich admin → create a user for them  
2. Send: `https://photos.pintoserve.com` + their Immich username/password  
3. No Cloudflare invites or email OTP required  

---

## Next: PhotoPrism + Ofelia

1. Find Immich library under `/mnt/user/photos/immich/library/`
2. Deploy PhotoPrism with that path mounted **read-only**
3. Add Ofelia to run `photoprism index --cleanup` on a schedule
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
- Do **not** put the Unraid management UI on the tunnel
- Use strong Immich passwords; create separate users for friends
- Keep Immich, cloudflared, and Unraid updated
- Optional later: Cloudflare Access + Immich custom proxy headers (service token) if you want an extra browser gate
