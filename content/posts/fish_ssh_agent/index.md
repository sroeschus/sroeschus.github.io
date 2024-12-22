---
title: "How to use ssh-agent with the fish shell"
description: "How to start ssh-agent with the fish-shell"
featuredImage: "fish.jpg"
toc:
  enable: false
tags: ["linux", "shell", "fish", "git", "git push", "ssh", "ssh-agent"]
date: 2024-12-21T19:27:16-08:00
draft: false
---

This article describes how to start the ssh-agent when using the fish shell.
<!--more-->

## Overview
When using github it is now a requirement to use the ssh-agent to login to the
service. Otherwise new commits cannot be pushed to the remote repository. The github
website only describes how to do set this up for the bash-like and c-like shells. If the
same command is used for a fish shell it fails:
```shell
╭─shr@shr in ~ took 13m51s
╰─λ eval "$(ssh-agent -s)"

fish: Unsupported use of '='. In fish, please use 'set SSH_AUTH_SOCK /tmp/ssh-XXXXXXAXsPUo/agent.9287'.
```

## Solution
The solution is to start the ssh-agent like for C-shells:

```shell
╭─shr@shr in repo: sroeschus.github.io/content/posts/stack_usage on  main [?] took 0s
╰─λ agent
Agent pid 7434
```

## Fish alias
To make it easier to start the ssh-agent, it can be convenient to define a new shell alias
to start the ssh agent:
```shell
╭─shr@shr in repo: sroeschus.github.io/content/posts/stack_usage on  main [?] took 0s
╰─λ alias --save agent="eval \$(ssh-agent -c)"
funcsave: wrote /home/shr/.config/fish/functions/agent.fish
```
