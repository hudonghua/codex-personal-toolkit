---
name: cc-switch-provider-routing
description: Use when working with CC Switch providers backed by third-party OpenAI-compatible or Claude-compatible gateways, especially when reading keys from E:\AI账号\密码.txt, classifying which models a key supports, adding keys to Codex or Claude in CC Switch, or fixing malformed provider settings_config JSON.
---

# CC Switch Provider Routing

Use this skill when the user wants to manage `CC Switch` providers from local key files, especially `E:\AI账号\密码.txt`.

## What this skill covers

- Read one or more `newapi_channel_conn` records from `E:\AI账号\密码.txt`
- Probe `/v1/models` to classify each key by model family
- Route GPT-style keys to `Codex` providers in `CC Switch`
- Route Claude-style keys to `Claude` providers in `CC Switch`
- Avoid malformed `settings_config` records in `C:\Users\t250c\.cc-switch\cc-switch.db`
- Verify whether the active provider actually propagated to `~/.codex` or `~/.claude`

## Fast workflow

1. Read `E:\AI账号\密码.txt` as UTF-8.
2. Split records on blank lines. Each record should be standalone JSON.
3. Probe `https://.../v1/models` with the candidate key.
4. Classify by returned model IDs:
   - GPT/OpenAI family: route to `app_type='codex'`
   - Claude family: route to `app_type='claude'`
5. Backup `C:\Users\t250c\.cc-switch\cc-switch.db` before editing.
6. Write provider rows with valid JSON in `settings_config`.
7. Re-check the row with `json.loads(...)` before telling the user it is ready.

## Classification rules

- If `/v1/models` returns IDs such as `gpt-5.4`, `gpt-5.5`, or other GPT/OpenAI IDs, treat the key as a `Codex`/OpenAI-compatible provider.
- If `/v1/models` returns IDs such as `claude-sonnet-*`, `claude-opus-*`, or `claude-haiku-*`, treat the key as a `Claude` provider.
- Do not assume one gateway only serves one family. Probe the actual key.
- If `/v1/models` works but a specific model call returns `403`, the key exists but lacks access to that model.
- If a model call returns `500` or `503`, treat that as gateway/server instability, not proof of model entitlement.

## Correct provider shapes

### Codex provider (`app_type='codex'`)

`settings_config` must be a JSON object shaped like:

```json
{
  "auth": {
    "auth_mode": "apikey",
    "OPENAI_API_KEY": "..."
  },
  "config": "model_provider = \"custom\"\nmodel = \"gpt-5.4\"\n\n[model_providers.custom]\nname = \"custom\"\nbase_url = \"https://api.example.com/v1\"\nwire_api = \"responses\"\nrequires_openai_auth = true\n"
}
```

Notes:

- `settings_config` must be valid JSON, not a PowerShell-looking object literal.
- The `config` value is TOML stored as a JSON string.
- For local disk propagation, `~/.codex/auth.json` should become `apikey` mode and `~/.codex/config.toml` should point to the gateway `/v1`.

### Claude provider (`app_type='claude'`)

`settings_config` must be a JSON object shaped like:

```json
{
  "env": {
    "ANTHROPIC_API_KEY": "...",
    "ANTHROPIC_BASE_URL": "https://api.example.com"
  }
}
```

Notes:

- Do not put Claude keys into `Codex`.
- `~/.claude/settings.json` should contain the same `env` keys after the provider is active.
- Provider switch and model switch are separate. A new key can be active while the session still uses an older Claude model string.

## Common failure modes

- `Codex 供应商配置必须是 JSON 对象`
  - Root cause: `settings_config` was written as invalid pseudo-JSON.
  - Fix: generate JSON inside Python with `json.dumps(...)`, then store that string in SQLite.

- `This token has no access to model ...`
  - Root cause: the key does not have entitlement for the selected model.
  - Fix: probe `/v1/models` and align the model selection with the returned family and access.

- User says they selected a new Claude key but errors still mention an older model such as `claude-opus-4-7`
  - Root cause: provider changed, but the active Claude session kept its previous `/model` selection.
  - Fix: start a new Claude session and set `/model` again, or explicitly set a new default model.

## Verification checklist

- Database row exists in `C:\Users\t250c\.cc-switch\cc-switch.db`
- `settings_config` parses with `json.loads(...)`
- `website_url` matches the gateway host
- `is_current` reflects the intended active provider
- `C:\Users\t250c\.cc-switch\settings.json` points to the intended current provider
- `~/.codex` or `~/.claude` matches the active provider after switching

## Script

Use `scripts/probe_newapi_keys.py` to classify keys from a local file before editing `CC Switch`.
