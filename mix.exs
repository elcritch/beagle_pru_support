defmodule Pru.Mixfile do
  use Mix.Project

  @app :beagle_pru_support

  def project do
    [
      name: @app,
      description:
        "Basic library that enables easy interaction with the PRU cores present in the BeagleBone Black.",
      app: @app,
      version: "0.6.0",
      nerves_package: nerves_package(),
      elixir: "~> 1.6",
      compilers: [:nerves_package] ++ Mix.compilers(),
      make_clean: ["clean"],
      start_permanent: Mix.env() == :prod,
      package: package(),
      aliases: [loadconfig: [&bootstrap/1]],
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  def aliases("host"), do: []

  def nerves_package do
    [
      name: @app,
      type: :extras_toolchain,
      platform: NervesExtras.Toolchain,
      toolchain_extras: [
        env_var: "PRU_LIB",
        build_link_path: "",
        clean_files: ["priv"],
        archive_script: "scripts/archive.sh"
      ],
      platform_config: [],
      target_tuple: :arm_unknown_linux_gnueabihf,
      artifact_sites: [
        {:github_releases, "elcritch/#{@app}"}
      ],
      checksum: package_files()
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:ex_doc, ">= 0.0.0", only: :dev},
      {:toolchain_extras, "~> 0.1", github: "elcritch/toolchain_extras", runtime: false},
      {:toolchain_extras_pru_cgt, "~> 2.2.1", github: "elcritch/extras_toolchain_pru_cgt"}
    ]
  end

  defp package do
    [
      maintainers: ["Mikel Cranfill", "Jaremy Creechley"],
      files: package_files(),
      licenses: ["MIT"],
      links: %{"Github" => "https://github.com/elcritch/pru"}
    ]
  end

  defp package_files() do
    [
      "Makefile",
      "LICENSE",
      "mix.exs",
      "README.md",
      "scripts",
      "pru/include/ReadMe.txt",
      "pru/lib/pru_support/Makefile",
      "pru/lib/rpmsg/Makefile",
      "pru/lib/msgpack/Makefile",
      "pru/lib/softspi/Makefile",
    ]
    ++ Path.wildcard("pru/**/*.c")
    ++ Path.wildcard("pru/**/*.h")
    ++ Path.wildcard("pru/**/*.cpp")
    ++ Path.wildcard("pru/**/*.hpp")
    ++ Path.wildcard("pru/**/Makefile")
  end

  def aliases() do
    [] |> Nerves.Bootstrap.add_aliases()
  end

  defp bootstrap(args) do
    System.put_env("MIX_TARGET", "CC")
    Application.start(:nerves_bootstrap)
    Mix.Task.run("loadconfig", args)
  end
end
