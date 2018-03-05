defmodule BeaglePruSupportTest do
  use ExUnit.Case
  doctest BeaglePruSupport

  test "greets the world" do
    assert BeaglePruSupport.hello() == :world
  end
end
