import { useState, useEffect } from "react";
import { submitLicense, onLicenseStatus } from "../bridge";
import { Button } from "@/components/ui/button";
import { Textarea } from "@/components/ui/textarea";
import { Label } from "@/components/ui/label";
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";

export default function LicenseOverlay({ onActivated }) {
  const [keyInput, setKeyInput] = useState("ZW1haWw9Y3VzdG9tZXJAZ21haWwuY29tO3Byb2R1Y3Q9RGVncmFkaXplcg==:5f9d7f689bd5a58c33322f4d3274c246d99507a9f2194541fa9dbf6e0376a80af77f397eef3b94b8e66bf1ae21a3f1eff7a5575e56c9db7e6a0e7d2660f0a9da00626261acd3fc7d7daa2a09c74c1c999eaef5f172781911925b4fcee1424f06dd07b94dde41cf4374bd192eb7743dea33198a42257aac59aca02edf5dd6191e");
  const [error, setError] = useState(false);

  useEffect(() => {
    return onLicenseStatus((status) => {
      if (status.valid) {
        onActivated?.();
      } else if (keyInput.length > 0) {
        setError(true);
      }
    });
  }, [keyInput, onActivated]);

  const handleActivate = () => {
    if (!keyInput.trim()) { setError(true); return; }
    setError(false);
    submitLicense(keyInput.trim());
  };

  return (
    <div className="absolute inset-0 z-[100] flex items-center justify-center rounded-[inherit] bg-black/88 ">
      <Card className="w-[400px] bg-black/25 border-none">
        <CardHeader className="pb-5">
          <CardTitle className="text-sm tracking-wide">Activate Budgetizer</CardTitle>
          <CardDescription className="text-[10px] leading-1">
            Paste the license key you received after purchase.
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-3">
          <div className="space-y-1.5">
            <Label htmlFor="license-key" className="sr-only text-[10px]">License key</Label>
            <Textarea
              id="license-key"
              value={keyInput}
              onChange={(e) => { setKeyInput(e.target.value); setError(false); }}
              onKeyDown={(e) => { if (e.key === "Enter" && !e.shiftKey) { e.preventDefault(); handleActivate(); } }}
              placeholder="Paste license key here…"
              className={`h-32 resize-none font-mono text-[10px] bg-input ${error ? "border-destructive" : "border-transparent"}`}
            />
            <p className={`text-[8px] text-destructive h-3 ${error ? "visible" : "invisible"}`}>
              Invalid license key. Please check and try again.
            </p>
          </div>
        </CardContent>
        <CardFooter>
          <Button onClick={handleActivate} variant="secondary" size={"sm"} className="w-full text-[10px]">
            Activate
          </Button>
        </CardFooter>
      </Card>
    </div>
  );
}
